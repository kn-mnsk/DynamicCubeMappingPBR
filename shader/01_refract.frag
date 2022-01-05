#version 430 core

out vec4 FragColor;

in VERT_OUT {
    vec3 MPos;
    vec3 Normal;
} frag_in;

float PI = 3.1415926535897;

uniform bool reflectanceSwitch;
uniform vec3 cameraPos; // camera position in world coords
uniform samplerCube captureCubemap;
uniform vec3 objectPos; // world coordinates

//  eta https://physicsabout.com/refraction-of-light/
//float eta = 1.00 / 2.42;//ratio of refractive indices, air / diamond 
float eta = 1.00 / 1.66 ; // ratio of refractive indices, air / flint glass 
//float eta = 1.00 / 1.52;//ratio of refractive indices, air / crown glass 
//float eta = 1.00 / 1.36;//ratio of refractive indices, air / ethyl alcohol 
//float eta = 1.00 / 1.33;//ratio of refractive indices, air / water 
//float eta = 1.00 / 1.31;//ratio of refractive indices, air / ice 

// reflectance
float refl;

//
float F(float eta, float c){

    float n  = 1.0 / eta;

	float g = sqrt(pow(n, 2.0) + pow(c, 2.0) - 1.0);
	
	float gMc = g - c;
	float gPc = g + c;
	float cg_nom  = c * gPc - 1.0;
	float cg_denom = c * gMc + 1.0;
	
	return (1.0 / 2.0) * pow(gMc / gPc, 2.0) * (1.0 + pow(cg_nom / cg_denom, 2.0));
}

float reflectance(float eta, vec3 v, vec3 normal){
	float c =dot(v,  normal);
	float f = F(eta, c);
	float refl = f + (1.0 - f) * pow(1.0 - c, 5.0);

return refl;
}

mat4 rotationMatrix(vec3 axis, float angle)
{
//http://www.neilmendoza.com/glsl-rotation-about-an-arbitrary-axis/
//
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}


void main()
{
	vec3 N1 = frag_in.Normal; // normalized
	vec3 P1 = frag_in.MPos;
	vec3 C = cameraPos;
	vec3 V1 = normalize(C - P1);
	
	// first refraction
	vec3 s1 = refract(-V1, N1, eta);
	s1 = normalize(s1);
	//vec3 refractColor  =vec3( textureLod(captureCubemap, s1, 0)); // first refraction point

	// sencod refraction(trial)
	vec3 axis = normalize(cross(s1, N1));
	vec3 axisRotate = normalize(cross(s1, axis));
	float angle = PI;
	mat4 rotMat = rotationMatrix(axisRotate, angle);
	rotMat = transpose(inverse(rotMat));
	vec3  N2 = -normalize(vec3(rotMat * vec4(N1, 0.0))); 
	vec3 s2 = refract(s1, N2, 1.0/eta);
	s2 = normalize(s2);
	vec3 refractColor  =vec3( textureLod(captureCubemap, s2, 0)); // second refraction point
	
	// reflection
	vec3 r = reflect(-V1, N1);
	r = normalize(r);
	vec3 reflectColor  = vec3(texture(captureCubemap, r));

	if (reflectanceSwitch) {
		 refl = reflectance(eta, V1, N1); 
	 } else {
		refl = 0.08; // glass
		//refl =  0.02; // water surface
	 }

	FragColor =  vec4(mix(refractColor, reflectColor,  refl), 1.0);
}
