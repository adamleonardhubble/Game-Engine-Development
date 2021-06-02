#region Vertex

#version 440 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_textureCoordinates;
layout (location = 2) in vec3 a_normals;

layout (std140) uniform FBMatrix
{
    mat4 u_viewProjection;
};

out vec2 v_textureCoordinates;

uniform bool u_chaos;
uniform bool u_confuse;
uniform bool u_shake;
uniform float u_timestepSCC;

void main()
{
    gl_Position = u_viewProjection * vec4(a_position.x, a_position.y, 0.0f, 1.0f); 
    vec2 texture = a_textureCoordinates;
	
    if (u_chaos)
    {
        float strength = 0.2;
		
        vec2 position = vec2(texture.x + sin(u_timestepSCC) * strength, texture.y + cos(u_timestepSCC) * strength);        
        v_textureCoordinates = position;
    }
    else if (u_confuse)
    {
	float strength = 0.2;
		
        vec2 position = vec2((1.0 - texture.x) + cos(u_timestepSCC) * strength, (1.0 - texture.y) + sin(u_timestepSCC) * strength);        
        v_textureCoordinates = position;
    }
    else
    {
        v_textureCoordinates = texture;
    }
    
    if (u_shake)
    {
        float strength = 0.01;
        gl_Position.x += cos(u_timestepSCC * 100) * strength;        
        gl_Position.y += cos(u_timestepSCC * 150) * strength;        
    }
}  


#region Fragment

#version 440 core

in  vec2 v_textureCoordinates;
out vec4 v_fragColor;
  
uniform sampler2D u_colourTexturePrimary;

uniform bool u_chaos;
uniform bool u_confuse;
uniform bool u_shake;

void main()
{
    float offset = 1.0f / 300.0f;
	
    vec2 offsets[9] = vec2[](
    vec2( -offset,  offset  ),  // top-left
    vec2(  0.0f,    offset  ),  // top-center
    vec2(  offset,  offset  ),  // top-right
    vec2( -offset,  0.0f    ),  // center-left
    vec2(  0.0f,    0.0f    ),  // center-center
    vec2(  offset,  0.0f    ),  // center-right
    vec2( -offset, -offset  ),  // bottom-left
    vec2(  0.0f,   -offset  ),  // bottom-center
    vec2(  offset, -offset  )   // bottom-right    
    );
	
    int edgeKernel[9] = {
        -1, -1,  -1,
        -1,  8,  -1,
        -1, -1,  -1
    };
    
    float blurKernel[9] = {
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
        2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
    };

    vec3 textureSample[9];
	
    // sample from texture offsets if using convolution matrix
    if(u_chaos || u_shake)
    {
        for(int i = 0; i < 9; i++)
	{
            textureSample[i] = vec3(texture(u_colourTexturePrimary, v_textureCoordinates.st + offsets[i]));
	}
    }

    // process effects
    if (u_chaos)
    {           
        for(int i = 0; i < 9; i++)
	{
            v_fragColor += vec4(textureSample[i] * edgeKernel[i], 0.0f);
	}
		
        v_fragColor.a = 1.0f;
    }
    else if (u_confuse)
    {
        v_fragColor = vec4(1.0 - texture(u_colourTexturePrimary, v_textureCoordinates).rgb, 1.0);
    }
    else if (u_shake)
    {
        for(int i = 0; i < 9; i++)
	{
            v_fragColor += vec4(textureSample[i] * blurKernel[i], 0.0f);
	}
		
        v_fragColor.a = 1.0f;
    }
    else
    {
        v_fragColor = texture(u_colourTexturePrimary, v_textureCoordinates);
    }
}