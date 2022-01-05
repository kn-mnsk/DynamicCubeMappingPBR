#version 430 core
layout(std140) uniform;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;

out vec4 outColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	outColor = aColor;
	gl_Position =  projection * view * model * vec4(aPos, 1.0);
//	mat4 rotView = mat4(mat3(view));
//	gl_Position =  projection * rotView *  vec4(aPos, 1.0);
   
}
