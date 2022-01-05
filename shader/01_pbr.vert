#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;


out VERT_OUT {
    vec3 MPos;
    vec2 TexCoord;
    vec3 Normal;
} vers_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vers_out.MPos = vec3(model * vec4(aPos, 1.0));
    vers_out.TexCoord = aTexCoord;
    //vers_out.Normal = vec3(transpose(inverse(model)) * vec4(aNormal, 0.0));
    vers_out.Normal =normalize( vec3(transpose(inverse(model)) * vec4(aNormal, 0.0)));
    //vers_out.Normal = normalize(vec3((inverse(model)) * vec4(aNormal, 0.0)));
    //vers_out.Normal = vec3(model) * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);

}