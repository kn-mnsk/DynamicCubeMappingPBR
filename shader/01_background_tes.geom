#version 430 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 projection;
uniform mat4 view;

in TESE_OUT {
    vec3 MPos;
    vec3 Normal;
    vec2 TexCoord;
} geom_in[];

out GEOM_OUT {
    vec3 MPos;
    vec3 Normal;
    vec2 TexCoord;
} geom_out;


void main() {    

    for(int i = 0; i < gl_in.length(); i++)
    {
        //gl_Position = gl_in[i].gl_Position;
        mat4 rotView = mat4(mat3(view));
        gl_Position =(projection * rotView *  vec4( geom_in[i].MPos, 1.0)).xyww;
        geom_out.MPos = geom_in[i].MPos;
        geom_out.Normal = geom_in[i].Normal;
        geom_out.TexCoord = geom_in[i].TexCoord;
        EmitVertex();
    }

    EndPrimitive();

}

