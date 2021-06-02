#region Vertex

#version 440 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_vertexNormal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_bitangent;

uniform mat4 u_lightSpaceMatrix;
uniform mat4 u_model;

void main()
{
    gl_Position = u_lightSpaceMatrix * u_model * vec4(a_position, 1.0);
}   


#region Fragment

#version 440 core

void main() 
{
   //gl_FragDepth = gl_FragCoord.z;
}