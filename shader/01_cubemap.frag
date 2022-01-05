#version 430 core
out vec4 FragColor;

in vec3 WorldPos;
in vec3 Normal;

uniform sampler2D background; // specular
uniform sampler2D reflection;  // irradiance
uniform sampler2D environment; // diffuse

uniform vec3 cameraPos;

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
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{		
    vec3 N = Normal;
    vec3 V = normalize(cameraPos - WorldPos);

    vec3 F0 = vec3(0.04);//vec3(0.04);
    vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);        
    vec3 kD = vec3(1.0) - kS;

    vec2 uv = SampleSphericalMap(normalize(WorldPos));

    vec3 bg = texture(background, uv).rgb;
    //bg = bg / (bg + vec3(1.0));
    //bg = pow(bg, vec3(1.0/2.2));

    vec3 ref = texture(reflection, uv).rgb;
    ref = pow(ref, vec3(1.0 /2.2));

    vec3 env = texture(environment, uv).rgb;
    env = pow(env, vec3(1.0 / 2.2));

    // ambient lighting (we now use IBL as the ambient term)
    vec3 diffuse =   kD * ref * env / PI ;
  
   vec3 specular = kS * bg;

   color = diffuse + specular;
   //color =  bg; // debug
   //color = color / (color + vec3(1.0));
    FragColor = vec4(color, 1.0);
}
