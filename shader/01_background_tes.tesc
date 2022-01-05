#version 430 core                                                                               
                                                                                                
// define the number of CPs in the output patch                                                 
layout (vertices = 3) out;                                                                      
                                                                                                
uniform float gTessellationLevel;                                                               

in VERT_OUT {
    vec3 MPos;
    vec3 Normal;
    vec2 TexCoord;
} tesc_in[];

out TESC_OUT {
    vec3 MPos;
    vec3 Normal;
    vec2 TexCoord;
} tesc_out[];

float GetTessLevel(float Distance0, float Distance1)
{
    float AvgDistance = (Distance0 + Distance1) / 2.0;

    if (AvgDistance <= 2.0) {
        return 10.0;
    }
    else if (AvgDistance <= 5.0) {
        return 7.0;
    }                                                                                           
    else {
        return 3.0;
    }
} 


void main()
{
    // Set the control points of the output patch 
    tesc_out[gl_InvocationID].MPos = tesc_in[gl_InvocationID].MPos;
    tesc_out[gl_InvocationID].Normal = tesc_in[gl_InvocationID].Normal; 
    tesc_out[gl_InvocationID].TexCoord = tesc_in[gl_InvocationID].TexCoord;

    // Calculate the distance from the camera to the three control points
//    float EyeToVertexDistance0 = distance(camPos, tesc_out[0].MPos);
//    float EyeToVertexDistance1 = distance(camPos, tesc_out[1].MPos);
//    float EyeToVertexDistance2 = distance(camPos, tesc_out[2].MPos);
//
    // Calculate the tessellation levels                                                     
    gl_TessLevelOuter[0] = gTessellationLevel;                                                  
    gl_TessLevelOuter[1] = gTessellationLevel;                                                  
    gl_TessLevelOuter[2] = gTessellationLevel;                                                  
    gl_TessLevelInner[0] = gTessellationLevel;             
}