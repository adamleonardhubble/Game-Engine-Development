/** \file makeShader.h
*/
#pragma once

#include <Engine.h>

#include <filesystem>
#include <iostream>
#include <fstream>

/**
\class MakeShader Contains a static function for making shaders when the game is run
*/
class MakeShader
{
public:
	//! Function to make a shader for an object depending on what the object has
	/*!
	\param hasNormalMap If the object has a normal map on it
	\param hasHeightMap If the object has a height map on it
	\return If a new shader was made and the filepath of the shader that the object will use
	*/
	static std::string makeShader(bool hasNormalMap, bool hasHeightMap)
	{
		// Make shader filepath
		std::string filepath = "assets/savedJSON/" + std::to_string((int)hasNormalMap) + std::to_string((int)hasHeightMap) + ".glsl";
		// If the shader already exists, return
		if (std::filesystem::exists(filepath))
		{
			return filepath;
		}
		// Make the new shader file (copy an empty text file)
		std::filesystem::copy("assets/savedJSON/Default/emptyShader.glsl", filepath);

		std::ofstream shader;
		shader.open(filepath); // Open the new file
		// Add necessary lines to the file depending on what the object has

		// Vertex shader
		shader << "#region Vertex\n";

		shader << "#version 440 core\n";
		// Vertex data
		shader << "layout(location = 0) in vec3 a_vertexPosition;\n";
		shader << "layout(location = 1) in vec3 a_vertexNormal;\n";
		shader << "layout(location = 2) in vec2 a_texCoord;\n";
		shader << "layout(location = 3) in vec3 a_tangent;\n";
		shader << "layout(location = 4) in vec3 a_bitangent;\n";
		// Data to pass to the fragment shader
		shader << "out vec3 FragPos;\n";
		shader << "out vec2 texCoord;\n";
		shader << "out vec4 fragPositionLightSpace;\n";

		if (hasNormalMap || hasHeightMap)
		{
			shader << "out vec3 TangentLightPos;\n";
			shader << "out vec3 TangentLightDir;\n";
			shader << "out vec3 TangentViewPos;\n";
			shader << "out vec3 TangentFragPos;\n";
		}
		if (!hasNormalMap)
		{
			shader << "out vec3 Normal;\n";
		}

		// Uniform variables
		shader << "layout(std140) uniform Matrices\n";
		shader << "{\n";
		shader << "	mat4 u_viewProjection;\n";
		shader << "};\n";

		shader << "uniform mat4 u_model;\n";
		shader << "uniform mat4 u_lightSpaceMatrix;\n";

		shader << "layout(std140) uniform LightPos\n";
		shader << "{\n";
		shader << "	vec3 u_lightPos;\n";
		shader << "	vec3 u_lightDir;\n";
		shader << "	vec3 u_viewPos;\n";
		shader << "};\n";

		// Main vertex shader function
		shader << "void main()\n";
		shader << "{\n";
		shader << "	FragPos = vec3(u_model * vec4(a_vertexPosition, 1.0));\n";
		shader << "	texCoord = a_texCoord;\n";

		if (hasNormalMap || hasHeightMap)
		{
			shader << "vec3 T = normalize(mat3(u_model) * a_tangent);\n";
			shader << "vec3 B = normalize(mat3(u_model) * a_bitangent);\n";
			shader << "vec3 N = normalize(mat3(u_model) * a_vertexNormal);\n";
			shader << "mat3 TBN = transpose(mat3(T, B, N));\n";

			shader << "TangentLightPos = TBN * u_lightPos;\n";
			shader << "TangentLightDir = TBN * u_lightDir;\n";
			shader << "TangentViewPos = TBN * u_viewPos;\n";
			shader << "TangentFragPos = TBN * FragPos;\n";
		}
		if (!hasNormalMap)
		{
			shader << "Normal = transpose(inverse(mat3(u_model))) * a_vertexNormal;\n";
		}

		shader << "fragPositionLightSpace = u_lightSpaceMatrix * vec4(FragPos, 1.0);\n";
		shader << "gl_Position = u_viewProjection * vec4(FragPos, 1.0);\n";
		shader << "}\n";

		// Fragment shader
		shader << "#region Fragment\n";

		shader << "#version 440 core\n";

		shader << "layout(location = 0) out vec4 colour;\n";
		// Data passed from the vertex shader
		shader << "in vec3 FragPos;\n";
		shader << "in vec2 texCoord;\n";
		shader << "in vec4 fragPositionLightSpace;\n";

		if (hasNormalMap || hasHeightMap)
		{
			shader << "in vec3 TangentLightPos;\n";
			shader << "in vec3 TangentLightDir;\n";
			shader << "in vec3 TangentViewPos;\n";
			shader << "in vec3 TangentFragPos;\n";
		}
		if (!hasNormalMap)
		{
			shader << "in vec3 Normal;\n";
		}
		// Uniform variables
		shader << "layout(std140) uniform LightPos\n";
		shader << "{\n";
		shader << "	vec3 u_lightPos;\n";
		shader << "	vec3 u_lightDir;\n";
		shader << "	vec3 u_viewPos;\n";
		shader << "};\n";

		shader << "layout(std140) uniform Light\n";
		shader << "{\n";
		shader << "	vec3 u_ambient;\n";

		shader << "	vec3 u_pointDiffuse;\n";
		shader << "	vec3 u_pointSpecular;\n";

		shader << "	vec3 u_dirDiffuse;\n";
		shader << "	vec3 u_dirSpecular;\n";

		shader << "	vec3 u_attenuation;\n";
		shader << "};\n";

		shader << "uniform vec3 matDiffuse;\n";
		shader << "uniform vec3 matSpecular;\n";
		shader << "uniform float matShininess;\n";

		shader << "uniform sampler2D u_diffuseTex;\n";
		shader << "uniform sampler2D u_specTex;\n";

		shader << "uniform sampler2D u_shadowDepth;\n";

		if (hasNormalMap)
		{
			shader << "uniform sampler2D u_normalMap;\n";
		}
		if (hasHeightMap)
		{
			shader << "uniform sampler2D u_depthMap;\n";
			shader << "uniform float u_heightScale;\n";

			// Parallax mapping function
			shader << "vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir)\n";
			shader << "{\n";
			shader << "	const float minLayers = 8;\n";
			shader << "	const float maxLayers = 32;\n";
			shader << "	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));\n";

			shader << "	float layerDepth = 1.0 / numLayers;\n";

			shader << "	float currentLayerDepth = 0.0;\n";

			shader << "	vec2 viewDirection = viewDir.xy;\n";
			shader << "	viewDirection.y = -viewDirection.y;\n";
			shader << "	vec2 P = viewDirection / viewDir.z * u_heightScale;\n";
			shader << "	vec2 deltaTexCoords = P / numLayers;\n";

			shader << "	vec2  currentTexCoords = texCoord;\n";
			shader << "	float currentDepthMapValue = texture(u_depthMap, currentTexCoords).r;\n";

			shader << "	while (currentLayerDepth < currentDepthMapValue)\n";
			shader << "	{\n";
			shader << "		currentTexCoords -= deltaTexCoords;\n";
			shader << "		currentDepthMapValue = texture(u_depthMap, currentTexCoords).r;\n";
			shader << "		currentLayerDepth += layerDepth;\n";
			shader << "	}\n";

			shader << "	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;\n";

			shader << "	float afterDepth = currentDepthMapValue - currentLayerDepth;\n";
			shader << "	float beforeDepth = texture(u_depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;\n";

			shader << "	float weight = afterDepth / (afterDepth - beforeDepth);\n";
			shader << "	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);\n";

			shader << "	return finalTexCoords;\n";
			shader << "}\n";
		}

		// Shadow function
		shader << "float ShadowCalculation(vec4 fragPosLightSpace)\n";
		shader << "{\n";
		shader << "	// Perform perspective divide\n";
		shader << "	vec3 projectionCoordinates = fragPosLightSpace.xyz / fragPosLightSpace.w;\n";

		shader << "	// Transform to [0,1] range\n";
		shader << "	projectionCoordinates = projectionCoordinates * 0.5 + 0.5;\n";

		shader << "	// Get closest depth value from light's perspective (using [0,1] range fragPosLight as coordinates)\n";
		shader << "	float closestDepth = texture(u_shadowDepth, projectionCoordinates.xy).r;\n";

		shader << "	// Get depth of current fragment from light's perspective\n";
		shader << "	float currentDepth = projectionCoordinates.z;\n";

		shader << "	float bias = 0.01f;\n";
		shader << "	// Check whether current fragment position is in shadow\n";
		shader << "	float shadow = (currentDepth - bias) > closestDepth ? 1.0 : 0.0;\n";

		shader << "	//if (pos.z > 1)\n";
		shader << "		//pos.z = 1;\n";

		shader << "	//float bias = 0.01f;\n";
		shader << "	//float shadow = (depth + bias) < pos.z ? 0.0 : 1.0;\n";

		shader << "	return shadow;\n";
		shader << "}\n";

		// Main fragment shader function
		shader << "void main()\n";
		shader << "{\n";

		// Find view and light directions
		if (hasNormalMap || hasHeightMap)
		{
			shader << "vec3 viewDir = normalize(TangentViewPos - TangentFragPos);\n";
			shader << "vec3 lightDir = normalize(TangentLightPos - TangentFragPos);\n";
		}
		else
		{
			shader << "vec3 viewDir = normalize(u_viewPos - FragPos);\n";
			shader << "vec3 lightDir = normalize(u_lightPos - FragPos);\n";
		}

		if (hasHeightMap)
		{
			// Find new texture coordinates
			shader << "vec2 newTexCoord = ParallaxMapping(texCoord, viewDir);\n";
			shader << "if (newTexCoord.x > 1.0 || newTexCoord.y > 1.0 || newTexCoord.x < 0.0 || newTexCoord.y < 0.0)\n";
			shader << "	discard;\n";
		}
		else
		{
			shader << "vec2 newTexCoord = texCoord;\n";
		}

		// Find normal
		if (hasNormalMap)
		{
			shader << "vec3 norm = texture(u_normalMap, newTexCoord).rgb;\n";
			shader << "norm = normalize(norm * 2.0 - 1.0);\n";

			shader << "norm.y = -norm.y;\n";
		}
		else
		{
			shader << "vec3 norm = normalize(Normal);\n";
		}
		
		// Point light calculation
		shader << "float diff = max(dot(lightDir, norm), 0.0);\n";

		shader << "vec3 halfwayDir = normalize(lightDir + viewDir);\n";
		shader << "float spec = pow(max(dot(norm, halfwayDir), 0.0), matShininess);\n";

		shader << "vec3 ambient = u_ambient * matDiffuse * vec3(texture(u_diffuseTex, newTexCoord));\n";
		shader << "vec3 diffuse = u_pointDiffuse * diff * matDiffuse * vec3(texture(u_diffuseTex, newTexCoord));\n";
		shader << "vec3 specular = u_pointSpecular * spec * matSpecular * vec3(texture(u_specTex, newTexCoord));\n";

		shader << "float distance = length(u_lightPos - FragPos);\n";
		shader << "float attenuation = 1.0 / (u_attenuation.x + u_attenuation.y * distance + u_attenuation.z * (distance * distance));\n";
		shader << "diffuse *= attenuation;\n";
		shader << "specular *= attenuation;\n";

		// Find light direction
		if (hasNormalMap || hasHeightMap)
		{
			shader << "lightDir = normalize(-TangentLightDir);\n";
		}
		else
		{
			shader << "lightDir = normalize(-u_lightDir);\n";
		}

		// Directional light calculation
		shader << "diff = max(dot(norm, lightDir), 0.0);\n";

		shader << "halfwayDir = normalize(lightDir + viewDir);\n";
		shader << "spec = pow(max(dot(norm, halfwayDir), 0.0), matShininess);\n";

		shader << "float shadow = ShadowCalculation(fragPositionLightSpace);\n";

		shader << "vec3 dirDiffuse = u_dirDiffuse * diff * matDiffuse * vec3(texture(u_diffuseTex, newTexCoord));\n";
		shader << "vec3 dirSpecular = u_dirSpecular * spec * matSpecular * vec3(texture(u_specTex, newTexCoord));\n";
		shader << "vec3 dir = (1.0 - shadow) * (dirDiffuse + dirSpecular);\n";

		shader << "colour = vec4((ambient + diffuse + specular) + dir, 1.0);\n";
		shader << "}\n";

		shader.close();

		return filepath;
	}
};