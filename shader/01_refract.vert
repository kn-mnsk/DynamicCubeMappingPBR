#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VERT_OUT {
    vec3 MPos;
    vec3 Normal;
} vert_out;

void main()
{
	//put vertex pos into world coordinates
	vert_out.MPos = vec3(model * vec4(aPos, 1.0));
	//vertex normal in world coords
	vert_out.Normal = normalize(vec3(transpose(inverse(model)) * vec4(aNormal, 0.0)));

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}