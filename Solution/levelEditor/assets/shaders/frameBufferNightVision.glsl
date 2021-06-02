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

in vec2 v_textureCoordinates;
out vec4 v_fragColor;
			
uniform sampler2D u_colourTexturePrimary; // Scene texture
uniform sampler2D u_noiseTexture; // Noise texture
uniform sampler2D u_maskTexture; // Mask texture

uniform float u_luminosityThreshold = 0.1;
uniform float u_colourAmplification = 5;
uniform float u_effectCoverage;

uniform float u_timestepNV;

void main ()
{
    vec4 finalColor;
    
    if (v_textureCoordinates.x < u_effectCoverage) 
    { 
       vec2 UVCoordinates;
       
       UVCoordinates.x = 0.05 * sin(u_timestepNV * 50.0);
       UVCoordinates.y = 0.05 * cos(u_timestepNV * 50.0);       
    
       float mask = texture2D(u_maskTexture, v_textureCoordinates.st).r;
       vec3 noise = texture2D(u_noiseTexture, v_textureCoordinates.st + UVCoordinates).rgb;
       vec3 colour = texture2D(u_colourTexturePrimary, v_textureCoordinates.st + (noise.xy * 0.005)).rgb; 

       float luminosity = dot(vec3(0.20, 0.60, 0.10), colour);
    	
       if (luminosity < u_luminosityThreshold)
       {
      	   colour *= u_colourAmplification;
       } 
  
       vec3 nightVisionColour = vec3(0.10, 0.95, 0.10);
    
       finalColor.rgb = (colour + (noise * 0.20)) * nightVisionColour * mask;
    }
    else
    {
       vec3 colour = texture2D(u_colourTexturePrimary, v_textureCoordinates.st).rgb;

       finalColor.rgb = colour;
    }

    v_fragColor = vec4(vec3(finalColor.rgb), 1.0);
}			