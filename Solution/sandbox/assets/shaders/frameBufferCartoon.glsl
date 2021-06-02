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

void main()
{
    gl_Position = u_viewProjection * vec4(a_position.x, a_position.y, 0.0f, 1.0f); 
    
    v_textureCoordinates = a_textureCoordinates;
}  


#region Fragment

#version 440 core

in vec2 v_textureCoordinates;
out vec4 v_fragColor;
  
uniform sampler2D u_colourTexturePrimary;
uniform vec2 u_screenResolution;

uniform float u_cartoonRate = 1;

float hueDistance(float hueOne, float hueTwo) 
{
    float delta = abs(hueOne - hueTwo);

    return min(delta, 1.0 - delta);
}

vec3 RGBToHSL(vec3 color) 
{    
    float minRGB = min(min(color.r, color.g), color.b); // Minimum RGB value
    float maxRGB = max(max(color.r, color.g), color.b); // Maximum RGB value
    float delta = maxRGB - minRGB; // Delta of RGB values
    float sum = maxRGB + minRGB; // Sum of RGB values
    
    // Initialise to gray, luminance with no chromatic colour 
    vec3 hsl = vec3(0.0, 0.0, sum * (1.0 / 2.0));

    // Chromatic data
    if (delta != 0.0) 
    {        
        // Saturation
        if (hsl.z > 0.5)
        {
            sum = 2.0 - sum;
        } 

        hsl.y = delta / sum;

        // Hue
        vec3 deltaRGB = ((vec3(maxRGB) - color) * vec3(1.0 / 6.0) / vec3(delta) + vec3(1.0 / 2.0));
        
        //if (color.r == maxRGB )
        hsl.x = deltaRGB.b - deltaRGB.g; // Hue

        if (color.g == maxRGB)
        {
            hsl.x = (1.0 / 3.0) + deltaRGB.r - deltaRGB.b; // Hue
        }
        else if (color.b == maxRGB)
        {
            hsl.x = (2.0 / 3.0) + deltaRGB.g - deltaRGB.r; // Hue
        }

        hsl.x = fract(hsl.x);
    }

    return hsl;
}

float hueToRGB(float f1, float f2, float hue) 
{
    hue = fract(hue);

    if (hue < (1.0 / 6.0))
    {
        return f1 + (f2 - f1) * 6.0 * hue;
    }
    else if (hue < (1.0 / 2.0))
    {
        return f2;
    }
    else if (hue < (2.0 / 3.0))
    {
        return f1 + (f2 - f1) * (-6.0 * hue + (6.0 * 2.0 / 3.0));
    }
    else
    {
        return f1;
    }
}

vec3 HSLToRGB(vec3 hsl) 
{
    vec3 rgb = vec3(hsl.z); // Initialise to gray (Luminance)

    if (hsl.y != 0.0) 
    {
        float f2 = hsl.z + hsl.y - hsl.y * hsl.z;

        if (hsl.z < 0.5)
        {
            f2 = hsl.z + hsl.z * hsl.y;
        }
            
        float f1 = 2.0 * hsl.z - f2;
        
        rgb.r = hueToRGB(f1, f2, hsl.x + (1.0/3.0));
        rgb.g = hueToRGB(f1, f2, hsl.x);
        rgb.b = hueToRGB(f1, f2, hsl.x - (1.0/3.0));
    }
    
    return rgb;
}

vec4 BW(vec4 color) 
{
    float luminance = dot(color.rgb, vec3(0.2125, 0.7154, 0.0721));

    return vec4(vec3(luminance), color.a);
}

float Luminance(vec3 color) 
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

#define floor(t, coefficient) (float(int(t * coefficient)) / coefficient)

vec3 px(const int x, const int y) 
{
    vec2 uv = (v_textureCoordinates.st + vec2(x, y) / u_screenResolution);

    return texture2D(u_colourTexturePrimary, uv).xyz;
}

vec3 edge(void) 
{
    vec3 hc = px(-1,-1) *  1.0 + px( 0,-1) *  2.0
            + px( 1,-1) *  1.0 + px(-1, 1) * -1.0
            + px( 0, 1) * -2.0 + px( 1, 1) * -1.0;

    vec3 vc = px(-1,-1) *  1.0 + px(-1, 0) *  2.0
            + px(-1, 1) *  1.0 + px( 1,-1) * -1.0
            + px( 1, 0) * -2.0 + px( 1, 1) * -1.0;

    float coefficient = pow(Luminance(vc * vc + hc * hc), 0.6);

    if (coefficient > 0.4)
    {
       return vec3(1) * u_cartoonRate;
    }

    return vec3(0);
}

vec3 cartoonBW(vec3 x) 
{
    x = BW(vec4(x, 1.0)).rgb;

    return vec3(float(int(x[0] * 8.0)) / 8.0);
}

vec3 cartoonRGB(vec3 x) 
{
  vec3 hsl = RGBToHSL(x);

  hsl[0] = floor(hsl[0], 30.0);
  hsl[1] = floor(hsl[1], 10.0);
  hsl[2] = floor(hsl[2], 10.0);

  return HSLToRGB(hsl);
}

void main(void) 
{ 
    v_fragColor = texture2D(u_colourTexturePrimary, v_textureCoordinates);
    vec4 color = vec4(cartoonRGB(v_fragColor.rgb), v_fragColor.a);
    color *= vec4(vec3(1) - edge(), 1.0);
    
    v_fragColor = color * u_cartoonRate + v_fragColor * (1.0 - u_cartoonRate);
}