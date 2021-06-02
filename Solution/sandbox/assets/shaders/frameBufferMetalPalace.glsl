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
     gl_Position = u_viewProjection * vec4(a_position.x, a_position.y, 0.0f, 1.0f); 	
  
     v_textureCoordinates = a_textureCoordinates;
}


#region Fragment

#version 440 core

in vec2 v_textureCoordinates;
out vec4 v_fragColor;
			
uniform sampler2D u_colourTexturePrimary; // Scene texture

uniform int u_activeMetal;

uniform float u_luminosityThreshold = 0.2;
uniform float u_colourAmplification = 5;
uniform float u_effectCoverage = 1;

// GenericNoiseTwo
float rand(vec2 n)
{ 
     return fract(sin(dot(n.xy, vec2(12.9898, 4.1414))) * 43758.5453);
}

// VoroNoise
vec3 hash3(vec2 p)
{
     vec3 q = vec3(dot(p, vec2(127.1, 311.7)), 
		   dot(p, vec2(269.5, 183.3)), 
		   dot(p, vec2(419.2, 371.9)));
	
     return fract(sin(q) * 43758.5453);
}

void main ()
{
    vec4 finalColor;
    vec2 UVCoordinates;
    float luminosity;
    vec3 overlayingColour;
           
    //rand(UVCoordinates); // GenericNoiseTwo
    hash3(UVCoordinates); // VoroNoise

    float mask = texture2D(u_colourTexturePrimary, v_textureCoordinates.st).r;
    vec3 noise = texture2D(u_colourTexturePrimary, v_textureCoordinates.st + UVCoordinates).rgb;
    vec3 colour = texture2D(u_colourTexturePrimary, v_textureCoordinates.st + (noise.xy * 0.5)).rgb;  

    if (u_activeMetal == 1)
    {
	luminosity = dot(vec3(0.95, 0.90, 0.0), colour);
	overlayingColour = vec3(1.0, 0.95, 0.01);    
    }
    else if (u_activeMetal == 2)	
    {
	luminosity = dot(vec3(0.85, 0.85, 0.85), colour);
	overlayingColour = vec3(0.90, 0.90, 0.90); 
    }
    else if (u_activeMetal == 3)
    {
	luminosity = dot(vec3(0.95, 0.75, 0.8), colour);
	overlayingColour = vec3(1.0, 0.80, 0.85); 
    }

    if (luminosity < u_luminosityThreshold)
    {
      	colour *= u_colourAmplification;
    } 
     
    finalColor.rgb = (colour + (noise * 0.6)) * overlayingColour * mask;

    v_fragColor = vec4(vec3(finalColor.rgb), 1.0);
}			