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

uniform sampler2D u_depthTexture; // Depth texture

in vec2 v_textureCoordinates;
out vec4 v_fragColor; 

float LinearizeDepth(vec2 uv)
{
  float nearClippingPlane = 1.0;
  float farClippingPlane = 100.0;
  float DepthZ = texture2D(u_depthTexture, uv).x;
  return (2.0 * nearClippingPlane) / (farClippingPlane + nearClippingPlane - DepthZ * (farClippingPlane - nearClippingPlane));	
}

void main() 
{ 

  float depth = LinearizeDepth(v_textureCoordinates);

  v_fragColor = vec4(vec3(depth), 1.0);
}		