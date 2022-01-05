#version 430 core

layout(triangles, equal_spacing, ccw) in;
//layout(triangles, equal_spacing, cw) in;

in TESC_OUT {
    vec3 MPos;
    vec3 Normal;
    vec2 TexCoord;
} tese_in[];

out TESE_OUT {
    vec3 MPos;
    vec3 Normal;
    vec2 TexCoord;
} tese_out;

//uniform mat4 projection;
//uniform mat4 view;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2) 
{
return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{ 
   // Interpolate the attributes of the output vertex using the barycentric coordinates
    tese_out.TexCoord = interpolate2D(tese_in[0].TexCoord, tese_in[1].TexCoord, tese_in[2].TexCoord);
    tese_out.Normal = interpolate3D(tese_in[0].Normal, tese_in[1].Normal, tese_in[2].Normal);
    tese_out.Normal = normalize(tese_out.Normal);
    tese_out.MPos = interpolate3D(tese_in[0].MPos, tese_in[1].MPos, tese_in[2].MPos);

   // gl_Position = projection * view * vec4(tese_out.MPos, 1.0);
}                                                                                               
