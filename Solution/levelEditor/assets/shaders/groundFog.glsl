uniform float k;            // (F dot C <= 0.0) 
varying float3 rhoV; 
varying float F_dot_V; 
varying float F_dot_P; 

void main() 
{    
	float4 color = ...;    // final color    // Calculate distance * rho using Equation (5)    
	float d = saturate(k - F_dot_P / abs(F_dot_V));    
	d *= length(rhoV);    // Calculate fog fraction and apply    
	float f = saturate(exp2(-d));    
	gl_FragColor.rgb = color.rgb * f + gl_FogParameters.color.rgb * (1.0 - f);
	gl_FragColor.a = color.a; 
}


uniform float3 aV;         // (a / 2) * V 
varying float c1;          // k * (F dot P + F dot C) varying float c2;          // (1 - 2k) * (F dot P) 
varying float F_dot_V; 

void main() 
{    
	float4 color = ...;    // final color    

	// Calculate g(P) using Equation (13)    
	float g = min(c2, 0.0);
    
	g = -length(aV) * (c1 - g * g / abs(F_dot_V));    // Calculate fog fraction and apply    
	float f = saturate(exp2(-g));   
	gl_FragColor.rgb = color.rgb * f + gl_FogParameters.color.rgb * (1.0 - f);
	gl_FragColor.a = color.a; 
}