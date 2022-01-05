#version 430 core
out vec4 FragColor;

in vec3 WorldPos;
in vec3 Normal;

uniform sampler2D image;

float halfPI = 1.5707963267948;
float PI = 3.1415926535897;
//float doublePI = 6.2831853071795;

//const vec2 invAtan = vec2(0.1591, 0.3183);
vec3 color;

vec2 SampleSphericalMap(vec3 pos)
{
    //vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    //uv *= invAtan;
    //uv += 0.5;
    // return uv;

    float u = (atan(pos.z, pos.x)/PI + 1.0) / 2.0 ;
    float v = (asin(pos.y) / halfPI + 1.0) / 2.0;  // assume radius = 1.0
    return vec2(u, v);   
}

void main()
{	
    vec2 uv = SampleSphericalMap(normalize(WorldPos));

    color = texture(image, uv).rgb;

    FragColor = vec4(color, 1.0);
}
