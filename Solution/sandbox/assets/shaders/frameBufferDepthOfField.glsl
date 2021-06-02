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

uniform sampler2D u_colourTexturePrimary; // Colour texture
uniform sampler2D u_depthTexture; // Depth texture

uniform vec2 u_screenResolution; // Screen resolution

in vec2 v_textureCoordinates;
out vec4 v_fragColor;

#define Pi 3.1415926535897932384626433

vec2 v_texel = vec2(1.0 / u_screenResolution);

uniform float u_focalDepth; // DoF focus range

float focalLength = 15.0; // Focal range
float focalStop = 5.0; // Focal stop range

float nearClippingPlane = 0.1; // Near clipping plane
float farClippingPlane = 100.0; // Far clipping plane

int ringSamples = 3; // Samples of the first ring
int ringCount = 3; // Ring amount

float circleOfConfusion = 0.03; // Circle of confusion size (millimetres - 35mm film = 0.03mm)

float maximumBlur = 1.0; // Maximum blur (clamped: 1.0 - 0.0)
float blurMultiplier = 3.0; // Blur multiplier

float highlightThreshold = 1.0; // Highlight threshold
float highlightGain = 100.0; // Highlight gain

float edgeBias = 0.1; // Edge bias (Bokeh)
float chromaticFringe = 0.5; // Chromatic fringing (Bokeh)

float noiseAmount = 0.0001; // Noise amount (dither)

bool depthBlur = true; // Blur the depth buffer
float depthBlurSize = 1.0; // Size of the depth buffer

   float blurDepth(vec2 coordinates)
   {
	float depth = 0.0;
	
	vec2 offset = vec2(v_texel.x, v_texel.y) * depthBlurSize;
	
        vec2 offsets[9] = vec2[](
            vec2(-offset.x, -offset.y),
            vec2( 0.0f,     -offset.y),
            vec2( offset.x, -offset.y),
            vec2(-offset.x,  0.0f),
            vec2( 0.0f,      0.0f),
            vec2( offset.x,  0.0f),
            vec2(-offset.x,  offset.y),
            vec2( 0.0f,      offset.y),
            vec2( offset.x,  offset.y)   
        );

	float blurKernelGaussian[9] = float[](
    	     1 / 16.0, 2 / 16.0, 1 / 16.0,
             2 / 16.0, 4 / 16.0, 2 / 16.0,
             1 / 16.0, 2 / 16.0, 1 / 16.0  
        );

	for(int i = 0; i < 9; i++)
	{
	     float depthTextureRed = texture2D(u_depthTexture, coordinates + offsets[i]).r;
	     depth += depthTextureRed * blurKernelGaussian[i];
	}
	
	return depth;
   }

   vec3 color(vec2 coordinates, float blur)
   {
	vec3 colour = vec3(0.0);
	
	colour.r = texture2D(u_colourTexturePrimary, coordinates + vec2(0.0, 1.0) * v_texel * chromaticFringe * blur).r;
	colour.g = texture2D(u_colourTexturePrimary, coordinates + vec2(-0.866, -0.5) * v_texel * chromaticFringe * blur).g;
	colour.b = texture2D(u_colourTexturePrimary, coordinates + vec2(0.866, -0.5) * v_texel * chromaticFringe * blur).b;
	
	vec3 luminosityCoefficient = vec3(0.299, 0.587, 0.114);
	float luminosity = dot(colour.rgb, luminosityCoefficient);
	float threshold = max((luminosity - highlightThreshold) * highlightGain, 0.0);
	
        return colour + mix(vec3(0.0), colour, threshold * blur);
   }

   vec2 randomiseNoise(vec2 coordinates)
   {
	float noiseX = ((fract(1.0 - coordinates.s * (u_screenResolution.x / 2.0)) * 0.25) + (fract(coordinates.t * (u_screenResolution.y / 2.0)) * 0.75)) * 2.0 - 1.0;
	float noiseY = ((fract(1.0 - coordinates.s * (u_screenResolution.x / 2.0)) * 0.75) + (fract(coordinates.t * (u_screenResolution.y / 2.0)) * 0.25)) * 2.0 - 1.0;

	return vec2(noiseX, noiseY);
   }

   float linearize(float depth)
   {
	return -farClippingPlane * nearClippingPlane / (depth * (farClippingPlane - nearClippingPlane) - farClippingPlane);
   }

   void main() 
   {
	float depth = linearize(texture2D(u_depthTexture, v_textureCoordinates.st).x);
	
	if (depthBlur)
	{
	    depth = linearize(blurDepth(v_textureCoordinates.st));
	}
	
	float blurAmount = 0.0;
	
        float f = focalLength;
        float d = u_focalDepth * 1000.0;
        float o = depth * 1000.0;

        float a = (o * f) / (o - f); 
	float b = (d * f) / (d - f); 
	float c = (d - f) / (d * focalStop * circleOfConfusion); 
		
	blurAmount = abs(a - b) * c;

	blurAmount = clamp(blurAmount, 0.0, 1.0);
	
	vec2 noise = randomiseNoise(v_textureCoordinates.st) * noiseAmount * blurAmount;
	
	float width = (1.0 / u_screenResolution.x) * blurAmount * maximumBlur * blurMultiplier + noise.x;
	float height = (1.0 / u_screenResolution.y) * blurAmount * maximumBlur * blurMultiplier + noise.y;
	
	vec3 colour = vec3(0.0);
	
        if (blurAmount < 0.05)
	{
	    colour = texture2D(u_colourTexturePrimary, v_textureCoordinates.st).rgb;
	}
	else
	{
	    colour = texture2D(u_colourTexturePrimary, v_textureCoordinates.st).rgb;
	    float sampleCount = 1.0;
	    int samples;
		
	    for (int i = 1; i <= ringCount; i += 1)
            {   
		 samples = i * ringSamples;
			
		 for (int j = 0 ; j < samples ; j += 1)   
		 {
		      float step = Pi * 2.0 / float(samples);
		      float pixelWidth = (cos(float(j) * step) * float(i));
		      float pixelHeight = (sin(float(j) * step) * float(i));
		      float pixelSize = 1.0;
		
		      colour += color(v_textureCoordinates.st + vec2(pixelWidth * width, pixelHeight * height), blurAmount) * mix(1.0, (float(i)) / (float(ringCount)), edgeBias) * pixelSize;  
		      sampleCount += 1.0 * mix(1.0, (float(i)) / (float(ringCount)), edgeBias) * pixelSize;   
		 }
	     }

	     colour /= sampleCount;
	}
	
	v_fragColor = vec4(vec3(colour), 1.0);
   }