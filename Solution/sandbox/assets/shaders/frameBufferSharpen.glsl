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
uniform int u_activeSharpen;
uniform vec2 u_screenResolution;

vec3 v_sampleTexture[9];
vec2 v_UVCoordinates;
vec2 v_offset5x5;

const float offset = 1.0 / 300.0;  

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float sharpenKernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    float unsharpMaskingKernel[25] = float[](
    	1 / 256.0,  4 / 256.0,    6 / 256.0,  4 / 256.0, 1 / 256.0,
        4 / 256.0, 16 / 256.0,   24 / 256.0, 16 / 256.0, 4 / 256.0,
        6 / 256.0, 24 / 256.0, -476 / 256.0, 24 / 256.0, 6 / 256.0,
        4 / 256.0, 16 / 256.0,   24 / 256.0, 16 / 256.0, 4 / 256.0,
        1 / 256.0,  4 / 256.0,    6 / 256.0,  4 / 256.0, 1 / 256.0
    );

    vec3 color3x3 = vec3(0.0);
    vec4 color5x5 = vec4(0.0);

    v_offset5x5.x = -2;
    v_offset5x5.y = -2;

    for (int i = 0; i < 9; i++)
    {
        v_sampleTexture[i] = vec3(texture(u_colourTexturePrimary, v_textureCoordinates.st + offsets[i]));
    }
 
    if (u_activeSharpen == 1)
    {
        for(int i = 0; i < 9; i++)
        {
            color3x3 += v_sampleTexture[i] * sharpenKernel[i];
        }

        v_fragColor = vec4(color3x3, texture(u_colourTexturePrimary, v_textureCoordinates).a);
    }
    else
    {
	for (int i = 0; i < 25; i++)
        {
            if (u_activeSharpen == 2)
	    {
		v_UVCoordinates.xy = v_offset5x5.xy / u_screenResolution.xy * 5.0f;
   
	        color5x5 += texture(u_colourTexturePrimary, vec2(v_textureCoordinates.x + v_UVCoordinates.x, v_textureCoordinates.y + v_UVCoordinates.y)) * unsharpMaskingKernel[i];
	    
		if (v_offset5x5.x > 2)
		{
		    v_offset5x5.y++;
		    v_offset5x5.x = -2;
		}
		
		v_offset5x5.x++;
	    }
        }

	 v_fragColor = vec4(color5x5.rgb, texture(u_colourTexturePrimary, v_textureCoordinates).a);
    }
}  
