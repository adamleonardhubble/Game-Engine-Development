#region Vertex

#version 440 core
			
layout(location = 0) in vec3 a_vertexPosition;
layout(location = 1) in vec3 a_vertexNormal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_bitangent;

out vec3 FragPos;
out vec2 texCoord;
out vec3 TangentLightPos;
out vec3 TangentLightDir;
out vec3 TangentViewPos;
out vec3 TangentFragPos;
out vec4 fragPositionLightSpace;

out vec3 Normal;

layout (std140) uniform Matrices
{
	mat4 u_viewProjection;
};

uniform mat4 u_model;
uniform mat4 u_lightSpaceMatrix;

layout (std140) uniform LightPos
{
	vec3 u_lightPos;
	vec3 u_lightDir;
	vec3 u_viewPos;
};

void main()
{
	FragPos = vec3(u_model * vec4(a_vertexPosition, 1.0));
	texCoord = a_texCoord;

	vec3 T = normalize(mat3(u_model) * a_tangent);
    vec3 B = normalize(mat3(u_model) * a_bitangent);
    vec3 N = normalize(mat3(u_model) * a_vertexNormal);
    mat3 TBN = transpose(mat3(T, B, N));
	
    TangentLightPos = TBN * u_lightPos;
	TangentLightDir = TBN * u_lightDir;
    TangentViewPos  = TBN * u_viewPos;
    TangentFragPos  = TBN * FragPos;

	Normal = transpose(inverse(mat3(u_model))) * a_vertexNormal;

	fragPositionLightSpace = u_lightSpaceMatrix * vec4(FragPos, 1.0);

	gl_Position =  u_viewProjection * vec4(FragPos, 1.0);
}
				
#region Fragment

#version 440 core
			
layout(location = 0) out vec4 colour;

in vec3 FragPos;
in vec2 texCoord;
in vec3 TangentLightPos;
in vec3 TangentLightDir;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in vec4 fragPositionLightSpace;

in vec3 Normal;

layout (std140) uniform LightPos
{
	vec3 u_lightPos;
	vec3 u_lightDir;
	vec3 u_viewPos;
};

layout (std140) uniform Light
{
	vec3 u_ambient;

	vec3 u_pointDiffuse;
	vec3 u_pointSpecular;

	vec3 u_dirDiffuse;
	vec3 u_dirSpecular;

	vec3 u_attenuation;
};

uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float matShininess;

uniform sampler2D u_diffuseTex;
uniform sampler2D u_specTex;
uniform sampler2D u_normalMap;
uniform sampler2D u_depthMap;

uniform sampler2D u_shadowDepth;

uniform bool u_hasNormalMap;
uniform bool u_hasDepthMap;

uniform float u_heightScale;

vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir);

float ShadowCalculation(vec4 fragPosLightSpace);

void main()
{
	vec3 norm = vec3(0);

	vec3 viewDir = vec3(0);
	vec3 lightDir = vec3(0);

	vec2 newTexCoord = texCoord;

	if (u_hasNormalMap || u_hasDepthMap)
	{
		viewDir = normalize(TangentViewPos - TangentFragPos);

		if (u_hasDepthMap)
		{
			newTexCoord = ParallaxMapping(newTexCoord, viewDir);
			if(newTexCoord.x > 1.0 || newTexCoord.y > 1.0 || newTexCoord.x < 0.0 || newTexCoord.y < 0.0)
				discard;
		}
		if (u_hasNormalMap)
		{
			norm = texture(u_normalMap, newTexCoord).rgb;
			norm = normalize(norm * 2.0 - 1.0);

			norm.y = -norm.y;
		}
		else
		{
			norm = normalize(Normal);
		}

		lightDir = normalize(TangentLightPos - TangentFragPos);
	}
	else
	{
		viewDir = normalize(u_viewPos - FragPos);
		norm = normalize(Normal);

		lightDir = normalize(u_lightPos - FragPos);
	}

    float diff = max(dot(lightDir, norm), 0.0);
    
	vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), matShininess);
    
    vec3 ambient  = u_ambient * matDiffuse * vec3(texture(u_diffuseTex, newTexCoord));
    vec3 diffuse  = u_pointDiffuse * diff * matDiffuse * vec3(texture(u_diffuseTex, newTexCoord));
    vec3 specular = u_pointSpecular * spec * matSpecular * vec3(texture(u_specTex, newTexCoord));

	float distance = length(u_lightPos - FragPos);
	float attenuation = 1.0 / (u_attenuation.x + u_attenuation.y * distance + u_attenuation.z * (distance * distance));  
	diffuse *= attenuation;
	specular *= attenuation;


	if (u_hasNormalMap || u_hasDepthMap)
	{
		lightDir = normalize(-TangentLightDir);
	}
	else
	{
		lightDir = normalize(-u_lightDir);
	}
	
    diff = max(dot(norm, lightDir), 0.0);
	
	halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(norm, halfwayDir), 0.0), matShininess);
    
	float shadow = ShadowCalculation(fragPositionLightSpace);

    vec3 dirDiffuse  = u_dirDiffuse * diff * matDiffuse * vec3(texture(u_diffuseTex, newTexCoord));
    vec3 dirSpecular = u_dirSpecular * spec * matSpecular * vec3(texture(u_specTex, newTexCoord));
	vec3 dir = (1.0 - shadow) * (dirDiffuse + dirSpecular);

	colour = vec4((ambient + diffuse + specular) + dir, 1.0);
}

vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir)
{ 
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  

    float layerDepth = 1.0 / numLayers;

    float currentLayerDepth = 0.0;

	vec2 viewDirection = viewDir.xy;
	viewDirection.y = -viewDirection.y;
    vec2 P = viewDirection / viewDir.z * u_heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    vec2  currentTexCoords     = texCoord;
    float currentDepthMapValue = texture(u_depthMap, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(u_depthMap, currentTexCoords).r;  
        currentLayerDepth += layerDepth;  
    }
    
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(u_depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;    
}


float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Perform perspective divide
    vec3 projectionCoordinates = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transform to [0,1] range
    projectionCoordinates = projectionCoordinates * 0.5 + 0.5;

    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coordinates)
    float closestDepth = texture(u_shadowDepth, projectionCoordinates.xy).r;
 
    // Get depth of current fragment from light's perspective
    float currentDepth = projectionCoordinates.z;

	float bias = 0.01f;
    // Check whether current fragment position is in shadow
    float shadow = (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
	
	//if (pos.z > 1)
		//pos.z = 1;
	
	//float bias = 0.01f;
	//float shadow = (depth + bias) < pos.z ? 0.0 : 1.0;

    return shadow;
}