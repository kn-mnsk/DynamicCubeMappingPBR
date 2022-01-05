#version 430 core
out vec4 FragColor;
in vec3 TexCoords;

uniform samplerCube environmentMap;

void main()
{		
    vec3 color = textureLod(environmentMap, TexCoords, 0.0).rgb;
    
    // HDR tonemap and gamma correct
    //color =color / (color + vec3(1.0));
    //color = pow(color, vec3(1.0/2.2)); 
    
    FragColor = vec4(color, 1.0);
}
