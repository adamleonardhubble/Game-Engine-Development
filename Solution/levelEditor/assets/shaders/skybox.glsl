#region Vertex

#version 440 core

layout (location = 0) in vec3 a_pos;

out vec3 texCoord;

uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
    texCoord = a_pos;
    vec4 pos = u_projection * u_view * vec4(a_pos, 1.0);
    gl_Position = pos.xyww;
}  

#region Fragment

#version 440 core

layout(location = 0) out vec4 colour;

in vec3 texCoord;

uniform samplerCube u_skybox;

void main()
{    
    colour = texture(u_skybox, texCoord);
}