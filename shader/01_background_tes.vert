#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;


out VERT_OUT {
    vec3 MPos;
    vec3 Normal;
    vec2 TexCoord;
} vert_out;

//uniform mat4 projection;
//uniform mat4 view;

void main()
{
    vert_out.MPos = aPos;
    vert_out.TexCoord = aTexCoord;
    vert_out.Normal = aNormal; 
//    vert_out.MPos =vec3( view * vec4(aPos, 1.0));
//    vert_out.TexCoord = aTexCoord;
//    vert_out.Normal = vec3(view * vec4(aNormal, 0.0));
    //gl_Position = projection * view  * vec4(aPos, 1.0);

}