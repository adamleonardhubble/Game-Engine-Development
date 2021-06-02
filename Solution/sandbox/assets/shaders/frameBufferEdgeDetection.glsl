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
uniform int u_activeEdgeDetection; // Active edge detection kernel

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

    float inverseKernelHigh[9] = float[](
    	1,  1,  1,
	1, -8,  1,
	1,  1,  1 
    );

    float inverseKernelMedium[9] = float[](
    	0,  1,  0,
	1, -4,  1,
	0,  1,  0 
    );
   
    float inverseKernelLow[9] = float[](
    	1,  0, -1,
	0,  0,  0,
       -1,  1,  1 
    );

    vec3 sampleTexture[9];

    for(int i = 0; i < 9; i++)
    {
        sampleTexture[i] = vec3(texture(u_colourTexturePrimary, v_textureCoordinates.st + offsets[i]));
    }

    vec3 color = vec3(0.0);

    for(int i = 0; i < 9; i++)
    {
        if (u_activeEdgeDetection == 1)
	{
            color += sampleTexture[i] * inverseKernelLow[i];
	}
	else if (u_activeEdgeDetection == 2)
	{
	    color += sampleTexture[i] * inverseKernelMedium[i];
	}
	else if (u_activeEdgeDetection == 3)
	{
            color += sampleTexture[i] * inverseKernelHigh[i];
	}
    }
    
    v_fragColor = vec4(color, texture(u_colourTexturePrimary, v_textureCoordinates).a);
}  
