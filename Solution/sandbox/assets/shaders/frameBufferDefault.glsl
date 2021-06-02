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
			
out vec4 v_fragColor;
in vec2 v_textureCoordinates;

uniform sampler2D u_colourTexturePrimary;

void main()
{
    v_fragColor = texture(u_colourTexturePrimary, v_textureCoordinates);
}
