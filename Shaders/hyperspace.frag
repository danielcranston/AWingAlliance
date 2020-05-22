#version 450

// Adapted from https://www.shadertoy.com/view/WdyXDR
// MIT License

uniform float time;
uniform vec2 resolution;

out vec4 out_Color;

const float tau = 6.28318;
const float period = 1.0;
const float speed  = 2.0/1.0;
const float rotation_speed = 2.5/1.0;
const float repeat_time = 1.0; // Length in seconds of the effect

float Hash(in vec2 p, in float scale)
{
	// This is tiling part, adjusts with the scale...
	p = mod(p, scale);
	return fract(sin(dot(p, vec2(27.16898, 38.90563))) * 5151.5473453);
}

//----------------------------------------------------------------------------------------
float Noise(in vec2 p, in float scale )
{
	vec2 f;
	p *= scale;
	f = fract(p);		// Separate integer from fractional
    
    p = floor(p);
    f = f*f*(3.0-2.0*f);	// Cosine interpolation approximation
	
    float res = mix(mix(Hash(p, 				 scale),
						Hash(p + vec2(1.0, 0.0), scale), f.x),
					mix(Hash(p + vec2(0.0, 1.0), scale),
						Hash(p + vec2(1.0, 1.0), scale), f.x), f.y);
    return res;
}

float fBm(in vec2 p)
{
    //p += vec2(sin(iTime * .7), cos(iTime * .45))*(.1) + iMouse.xy*.1/iResolution.xy;
	float f = 0.0;
	// Change starting scale to any integer value...
	float scale = 40.0;
    p = mod(p, scale);
	float amp   = 0.6;
	
	for (int i = 0; i < 5; i++)
	{
		f += Noise(p, scale) * amp;
		amp *= 0.5;
		// Scale must be multiplied by an integer value...
		scale *= 2.0;
	}
	// Clamp it just in case....
	return min(f, 1.0);
}


void main(void)
{
    float t = mod(time, repeat_time);
    t = t / repeat_time; // Normalized time
    
    vec4 col = vec4(0.0);
	vec2 q = gl_FragCoord.xy / resolution.xy;
	vec2 p = ( 2.0 * gl_FragCoord.xy - resolution.xy ) / min( resolution.y, resolution.x );
    // vec2 mo = (2.0 * iMouse.xy - resolution.xy) / min(resolution.x, resolution.y);
    p += vec2(0.0, -0.1);
    
    float ay = 0.0, ax = 0.0, az = 0.0;
    // if (iMouse.z > 0.0) {
    //     ay = 3.0 * mo.x;
    //     ax = 3.0 * mo.y;
    // }
    mat3 mY = mat3(
         cos(ay), 0.0,  sin(ay),
         0.0,     1.0,      0.0,
        -sin(ay), 0.0,  cos(ay)
    );
    
    mat3 mX = mat3(
        1.0,      0.0,     0.0,
        0.0,  cos(ax), sin(ax),
        0.0, -sin(ax), cos(ax)
    );
    mat3 m = mX * mY;
    
    vec3 v = vec3(p, 1.0);
    v = m * v;
    float v_xy = length(v.xy);
    float z = v.z / v_xy;
    
    // The focal_depth controls how "deep" the tunnel looks. Lower values
	// provide more depth.
	float focal_depth = 0.15;
    
    vec2 polar;
    float p_len = length(v.xy);
    polar.y = z * focal_depth + time * speed;
    float a = atan(v.y, v.x);
    a -= time * rotation_speed;
    float x = fract(a / tau);
    polar.y /= 3.0;
    polar.x = x * period + polar.y;
    
    
    // // Colorize blue
    float val = fBm(polar);
    col.rgb = vec3(0.15, 0.4, 0.9) * vec3(val)*1.3;
    
    // // Add white spots
    vec3 white = 0.5 * vec3(smoothstep(.8, 1.0, val));
    col.rgb += white;
        
    // // Add the white disk at the center
    float disk_size = 0.025;
    float disk_col = exp(-(p_len*1.5 - disk_size) * 2.);
    //col.rgb += mix(col.xyz, vec3(1,1,1), disk_col);
    col.rgb += vec3(disk_col, disk_col, disk_col);
    
    out_Color = vec4(col.rgb, 1.0);
}
