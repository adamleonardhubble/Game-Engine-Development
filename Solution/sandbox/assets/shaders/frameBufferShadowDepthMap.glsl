#region Vertex

#version 440 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_textureCoordinates;
layout (location = 2) in vec3 a_normals;

out vec2 v_textureCoordinates;

layout (std140) uniform FBMatrix
{
     mat4 u_viewProjection;
};

void main()
{
    gl_Position = u_viewProjection * vec4(a_position.x, a_position.y, 0.0, 1.0);
 
    v_textureCoordinates = a_textureCoordinates;
}   


#region Fragment

#version 440 core

uniform sampler2D u_depthMapTexture; // Depth map texture

uniform float u_nearPlane;
uniform float u_farPlane;

in vec2 v_textureCoordinates;
out vec4 v_fragColor;

float LinearizeDepth(vec2 uv)
{
    float nearPlane = 1.0;
    float farPlane = 100.0;
    float z = texture2D(u_depthMapTexture, uv).x;
    
    return (2.0 * u_nearPlane) / (u_farPlane + u_nearPlane - z * (u_farPlane - u_nearPlane));	
}

void main() 
{
    //float depthValue = LinearizeDepth(v_textureCoordinates);
    float depthValue = texture(u_depthMapTexture, v_textureCoordinates).r;

    //v_fragColor = vec4(vec3(LinearizeDepth(depthValue) / u_farPlane), 1.0);
    v_fragColor = vec4(vec3(depthValue), 1.0);
}