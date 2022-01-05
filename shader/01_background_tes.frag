#version 430 core
out vec4 FragColor;

in GEOM_OUT {
    vec3 MPos;
    vec3 Normal;
    vec2 TexCoord;
} frag_in;

uniform samplerCube environmentMap;

void main()
{		
    vec3 color = textureLod(environmentMap, frag_in.MPos, 0.0).rgb;
    
    // HDR tonemap and gamma correct
    //envColor = envColor / (envColor + vec3(1.0));
    //envColor = pow(envColor, vec3(1.0/2.2)); 
    //envColor = pow(envColor, vec3(2.2)); 
    
    FragColor = vec4(color, 1.0);
}
