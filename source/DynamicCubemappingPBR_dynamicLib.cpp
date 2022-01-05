#include <cassert>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//#include <tutorial/Khongton/GLSL.h>
#include <LearnOpenGL/camera.h>

#include <local_configuration/root_directory.h>
#include <custom/learnopengl/myshader.h>
#include <custom/learnopengl/mymodel.h>
#include <mydevelop/mytools2.h>


// This function is called when a GLFW error occurs
void callback_Error(int error, const char* description);
//void callback_Key(GLFWwindow* window, int key, int scancode, int action, int mods);
//static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
//void callback_CursorPos(GLFWwindow* window, double xpos, double ypos);
//void callback_Char(GLFWwindow* window, unsigned int key);
//void callback_Window_size(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
//void callback_Scroll(GLFWwindow* window, double xoffset, double yoffset);

//bool switchColorCorrection{ true };

void genCaptureCubemaps();

void genEnvCubemap();

void genIrradianceCubemaps();
void updateIrradianceCubemaps(int objectid);

void genPrefilterCubemaps();
void updatePrefilterCubemaps(int objectid);

void genBrdfLutTexture();
void updateBrdfLutTexture();

void init();
void renderOnFB(int objectid, bool pbrswitch, bool reflectanceswitch);
void renderAllOnScreen(bool pbrswitch, bool reflectanceswitch);

int tooglePolygonMode{ 0 };

GLenum polygonMode[3] = {
	GL_FILL,
	GL_LINE,
	GL_POINT,
};

// settings
int g_displayWidth{ 1560 };
int g_displayHeight{ SCR_HEIGHT };

// tessellation only
float tessellationLevel = 5.0f;  // cubemap tessellation

float bendNormal = 0.0f;
// tessellation only

float dispFactor = 0.0f;

meshVertices meshPatches{};
LineVertices axesMesh{};
LineVertices  normalLines{};
// GL Buffers setting
GLuint VAO, VBO, EBO, LVAO, LVBO, LEBO;


// camera
Camera* camera;// (glm::vec3(0.0f, 0.0f, 15.0f));
float nearClipPlane{ 0.1f };
float farClipPlane{ 1.1f };

float currentTime{};
float deltaTime{ 0.0f };
float lastFrame{ 0.0f };

float timef{ 0.0 };

GLFWwindow* window; // Main application window

MyShader::Shader* progImageToCubemap;
MyShader::Shader* progRefract;
MyShader::Shader* progBG;
MyShader::Shader* progBG_tes;
MyShader::Shader* progAxes;
MyShader::Shader* prog;
MyShader::Shader* progPBR;
MyShader::Shader* progPBR2;
MyShader::Shader* progIrradiance;
MyShader::Shader* progPrefilter;
MyShader::Shader* progBRDF;

GLint   sidelength;
GLuint* BufOutPtr;
GLfloat* BufOutFloatPtr;
size_t sideLength;
size_t size;

MyModel::Model* sphere;
MyModel::Model* cube;

bool keyToggles[256] = { false }; // only for English keyboards!

//GLuint hdrTexture;
GLuint TextureBG;
GLuint hdrTextureEnv;
GLuint hdrTextureRefl;

GLuint captureFBO;
GLuint captureRBO;
int g_captureFboWidth{ 3072 };// { 512 };
int g_captureFboHeight{ 3072 };// { 512 };

GLuint irradianceFBO;
GLuint irradianceRBO;
int g_irradianceFboWidth{ 32 };// { 32 };
int g_irradianceFboHeight{ 32 }; //{ 32 };

GLuint prefilterFBO;
GLuint prefilterRBO;
int g_prefilterFboWidth{ 128 }; //{ 128 };
int g_prefilterFboHeight{ 128 }; //{{ 128 };

GLuint brdfLutFBO;
GLuint brdfLutRBO;
int g_brdfLutFBOWidth{ 512 };//{ 512 };
int g_brdfLutFBOHeight{ 512 };//{ 512 };

const int numObjects{ 3 };
//const int arrayMapSize = numObjects * 2;
const int arrayMapSize = numObjects;

GLuint envCubemap;
GLuint captureCubemaps[arrayMapSize];
GLuint irradianceCubemaps[arrayMapSize];
GLuint prefilterCubemaps[arrayMapSize];
GLuint brdfLutTexture;

bool pbrSwitch{ false };
bool pbrOn{ true };
bool pbrOff{ false };

bool reflectanceOn{ true };
bool reflectanceOff{ false };

bool tessellationSwitch{ false };
bool refractionSwitch{ false };

struct IBLFILEDEF {
	std::string filename;;
	GLuint textureid;
	GLenum internalformat;
	GLenum format;
	GLenum type;
};

std::vector<IBLFILEDEF> iblFile = {
	{"textures/hdr/Mono_Lake_C/Mono_Lake_C_HiRes.jpg",  0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE},
	{"textures/hdr/Mono_Lake_C/Mono_Lake_C_Ref.hdr", 0, GL_RGB16F, GL_RGB, GL_FLOAT},
	{"textures/hdr/Mono_Lake_C/Mono_Lake_C_Env.hdr", 0, GL_RGB16F, GL_RGB, GL_FLOAT}
};


GLuint pbrTextures[numObjects][5];
std::vector<std::vector<std::string>> pbfTexFile = {
	{"textures/pbr/metal/gold/albedo.png", "textures/pbr/metal/gold/normal.png",
   "textures/pbr/metal/gold/metallic.png",  "textures/pbr/metal/gold/roughness.png",
	"textures/pbr/metal/gold/ao.png"},
	{"textures/pbr/metal/copper/albedo.png", "textures/pbr/metal/copper/normal.png",
   "textures/pbr/metal/copper/metallic.png",  "textures/pbr/metal/copper/roughness.png",
	"textures/pbr/metal/copper/ao.png"},
	{"textures/pbr/metal/titanium/albedo.png", "textures/pbr/metal/titanium/normal.png",
   "textures/pbr/metal/titanium/metallic.png",  "textures/pbr/metal/titanium/roughness.png",
	"textures/pbr/metal/titanium/ao.png"}
};


GLuint cubeMapVAO;
GLuint cubeMapVBO;
GLuint quadVAO;
GLuint quadVBO;

struct lookAtPara {
	glm::vec3 eyePos;
	glm::vec3 viewDirectionVec;
	glm::vec3 upVec;
};

std::vector<lookAtPara> captureViewPara = { // eyePos: default Position
	{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)}, // right
	{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)}, // left
	{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)}, // top
	{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)}, // bottom
	{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, 1.0f), glm::vec3(0.0f, -1.0f,  0.0f)}, // back
	{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)} // front
};

glm::mat4 getCaptureViewMat(int face, glm::vec3 eyepos) {

	glm::vec3 centerPos = eyepos + captureViewPara[face].viewDirectionVec;

	return glm::lookAt(eyepos, centerPos, captureViewPara[face].upVec);
}


glm::mat4 captureProjection(float fovdeg, float nearclipplane, float farclipplane) {

	return glm::perspective(glm::radians(fovdeg), 1.0f, nearclipplane, farclipplane);
}


// Matrix initializing
glm::mat4 P{ 1.0f };
glm::mat4 V{ 1.0f };
glm::vec3 cameraPos{ 0.0f }; // camera position in world coords is in the last column
glm::vec3 currentTarget{ 0.0f };
glm::vec3 lastTarget{ 0.0f };
glm::vec3 deltaTarget{ 0.0f };
glm::vec3 currentDirection{ 0.0f };
glm::vec3 lastDirection{ 0.0f };
glm::vec3 deltaDirection{ 0.0f };
float distance{ 0.0f };
glm::mat4 M{ 1.0f };
//

std::vector<glm::vec3> objectPos{};

glm::vec3 mainObjPos{ 0.0f, 0.0f, 0.0f };
std::vector<glm::vec3> getObjectPos(float timef) {
	std::vector<glm::vec3> pos = {
	mainObjPos,
	mainObjPos + 	glm::vec3{ 2.5f * cosf(timef), 2.5f * sinf(timef) , 0.0f},
	mainObjPos + 	glm::vec3{4.0f * sinf(timef), 0.0f, 4.0f * cosf(timef)} };
	return pos;
};


std::vector<glm::vec3> objectScale = {
	glm::vec3{ 1.0f },
	//glm::vec3{ 0.5f },
	glm::vec3{ 0.7f },
	glm::vec3{ 0.7f }
};

std::vector<float> farClipPlaneByObject = {
	30.0f,//20.0f,//
	30.0f, //30.0f,//
	40.0f,//40.0f,//
};


int main() {

	// glfw: initialize and configure
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_DEPTH_BITS, 16);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // only for 3.2 and above
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);


	// Set error callback.
	glfwSetErrorCallback(callback_Error);
	// Initialize the library.

	// Create a windowed mode window and its OpenGL context.

	std::vector<std::string> program_info = Get_Program_Infos();
	std::string title = "Dynamic Cubemapping  - " + program_info.at(0);
	window = glfwCreateWindow(g_displayWidth, g_displayHeight, title.c_str(), NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//glfwMaximizeWindow(window); 

	showOpenGLInfo(GL_VERSION);
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	//GLSL::checkVersion();

	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, callback_Key);
	// Set char callback.
	//glfwSetCharCallback(window, callback_Char);
	// Set cursor position callback.
	glfwSetCursorPosCallback(window, callback_CursorPos);
	// Set mouse button callback.
	//glfwSetMouseButtonCallback(window, callback_MouseButton);
	// Set the window resize call back.
	glfwSetFramebufferSizeCallback(window, callback_WindowSize);

	glfwSetScrollCallback(window, callback_Scroll);

	glfwMaximizeWindow(window);
	glfwGetWindowSize(window, &g_displayWidth, &g_displayHeight);

	// Initialize scene.
	init();

	/*Write textures as image on disk for debug*/
	// 1bmp file
	//writeTexture2D_bmp(brdfLutTexture, g_brdfLutFBOWidth, g_brdfLutFBOHeight, "other/brdfLutTexture");

	// 2hdr file
	//writeCubemap_hdr(envCubemap, g_captureFboWidth, g_captureFboHeight, "other/envCubemap");

	// 3png file
	/*writeCubemap_png(envCubemap, g_captureFboWidth, g_captureFboHeight, "other/envCubemap");
	writeCubemap_png(captureCubemaps[0], g_captureFboWidth, g_captureFboHeight, "other/captureCubemap_gold_side");
	writeCubemap_png(captureCubemaps[1], g_captureFboWidth, g_captureFboHeight, "other/captureCubemap_copper_side");*/
	/*writeCubemap_png(irradianceCubemaps[1], g_irradianceFboWidth, g_irradianceFboHeight, "other/irradianceCubemap_copper_side");
	writeCubemap_png(prefilterCubemaps[1], g_prefilterFboWidth, g_prefilterFboHeight, "other/prefilterCubemap_copper_side");*/

	// Loop until the user closes the window.
	while (!glfwWindowShouldClose(window)) {

		// per-frame time logic
		currentTime = (float)glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;
		timef = currentTime / PI;
		/*
		deltaTarget = (currentTarget - lastTarget);
		mainObjPos += distance * deltaTarget;
		distance = glm::length(mainObjPos - camera->Position);*/

		objectPos = getObjectPos(timef);

		/*lastTarget = currentTarget;
		currentTarget = camera->Position + camera->Front;*/

		// Set background color.
		glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (pbrSwitch) {// use PBR
			for (int objectid = 0; objectid < numObjects; objectid++) {
				renderOnFB(objectid, pbrSwitch, reflectanceOff); //  reflectanceOff to avoid unitended color in refraction while renderOnFB
				updateIrradianceCubemaps(objectid);
				updatePrefilterCubemaps(objectid);
			}
			renderAllOnScreen(pbrOff, reflectanceOn);  // no problem with color in refraction while reflectanceOn
		}
		else {// No PBR as Default
			for (int objectid = 0; objectid < numObjects; objectid++) {
				renderOnFB(objectid, pbrSwitch, reflectanceOff);// reflectanceOff to avoid unitended color in refraction while renderOnFB
			}
			renderAllOnScreen(pbrOff, reflectanceOn);
		}


		//writeCubemap_png(envCubemap, g_captureFboWidth, g_captureFboHeight, "other/envCubemap");
		//writeCubemap_png(captureCubemaps[0], g_captureFboWidth, g_captureFboHeight, "other/captureCubemap_gold_side");
		//writeCubemap_png(captureCubemaps[1], g_captureFboWidth, g_captureFboHeight, "other/captureCubemap_copper_side");
		//writeCubemap_png(irradianceCubemaps[1], g_irradianceFboWidth, g_irradianceFboHeight, "other/irradianceCubemap_copper_side");
		//writeCubemap_png(prefilterCubemaps[1], g_prefilterFboWidth, g_prefilterFboHeight, "other/prefilterCubemap_copper_side");

		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	// deletes  buffers & textures
	glDeleteFramebuffers(1, &captureFBO);
	glDeleteRenderbuffers(1, &captureRBO);
	glDeleteFramebuffers(1, &irradianceFBO);
	glDeleteRenderbuffers(1, &irradianceRBO);
	glDeleteFramebuffers(1, &prefilterFBO);
	glDeleteRenderbuffers(1, &prefilterRBO);
	glDeleteFramebuffers(1, &brdfLutFBO);
	glDeleteRenderbuffers(1, &brdfLutRBO);

	glDeleteTextures(1, &envCubemap);
	glDeleteTextures(arrayMapSize, &irradianceCubemaps[0]);
	glDeleteTextures(arrayMapSize, &prefilterCubemaps[0]);
	glDeleteTextures(1, &brdfLutTexture);
	glDeleteTextures(1, &TextureBG);
	glDeleteTextures(1, &hdrTextureEnv);
	glDeleteTextures(1, &hdrTextureRefl);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

// This function is called when a GLFW error occurs
void callback_Error(int error, const char* description) {
	std::cerr << description << std::endl;
}

// This function is called when a key is pressed
void callback_Key(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE: {
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		}
		case GLFW_KEY_F: {
			tooglePolygonMode += 1;
			tooglePolygonMode = tooglePolygonMode % 3;
			glPolygonMode(GL_FRONT_AND_BACK, polygonMode[tooglePolygonMode]);
			break;
		}
		case GLFW_KEY_R: {  // do not work ; conversion error: substitute for callback_Char
			refractionSwitch = !refractionSwitch;
			std::cout << (refractionSwitch ? "Refraction ON" : "Refraction OFF ") << std::endl;
			break;
		}
		}
	}

	// Alt +key: Tessellation
	//if (mods == GLFW_MOD_ALT && glfwGetKey(window, key) == GLFW_PRESS) {
	if (mods == GLFW_MOD_ALT && action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_P: {
			pbrSwitch = !pbrSwitch;
			std::cout << (pbrSwitch ? "PBR on FB" : "NO PBR on FB ") << std::endl;
			break;
		}
		case GLFW_KEY_T: {
			tessellationSwitch = !tessellationSwitch;
			std::cout << (tessellationSwitch ? "Tessellation On " : "Tesselation Off ") << std::endl;
			break;
		}
		case GLFW_KEY_DOWN: {
			tessellationLevel -= 1.0f;
			tessellationLevel = glm::clamp(tessellationLevel, 1.0f, 20.0f);
			break;
		}
		case GLFW_KEY_UP: {
			tessellationLevel += 1.0f;
			tessellationLevel = glm::clamp(tessellationLevel, 1.0f, 20.0f);
			break;
		}
		}
	}

	if (action == GLFW_REPEAT) {
		//if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_W: {
			camera->ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
			break;
		}
		case GLFW_KEY_S: {
			camera->ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
			break;
		}
		case GLFW_KEY_A: {
			camera->ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
			break;
		}
		case GLFW_KEY_D: {
			camera->ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
			break;
		}
		}
	}

}

// This function is called when the mouse moves
void callback_CursorPos(GLFWwindow* window, double xpos, double ypos) {
	//std::cout << "callback_CursorPos: " << xpos << ", " << ypos <<std::endl;
	double xposd = xpos - g_displayWidth / 2.0;
	double yposd = -ypos + g_displayHeight / 2.0;

	if (firstMouse)
	{
		lastX = (float)xposd;
		lastY = (float)yposd;
		firstMouse = false;
	}

	float xoffset = xposd - lastX;
	float yoffset = yposd - lastY; // reversed since y-coordinates go from bottom to top

	lastX = xposd;
	lastY = yposd;

	if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) && (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)){
		//camera->Position += camera->MovementSpeed * 0.1f * deltaTime * glm::vec3(xoffset, yoffset, 0.0f);
		camera->Position += camera->MouseSensitivity * (-yoffset) * camera->Up;;
		//camera->Position = glm::clamp(camera->Position, glm::vec3(-5.0f, -5.0f, 9.0f), glm::vec3(5.0f, 5.0f, 11.0f));
		//std::cout << camera->Position.x << ", " << camera->Position.y << ", " << camera->Position.z << std::endl;

		//deltaTarget = (currentTarget - lastTarget);
		////mainObjPos += distance * deltaTarget;
		//mainObjPos = distance * deltaTarget;
		//distance = glm::length(mainObjPos - camera->Position);

		//lastTarget = currentTarget;
		//currentTarget = camera->Position + camera->Front;

		return;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
		camera->ProcessMouseMovement(xoffset, yoffset);

		//deltaTarget = (currentTarget - lastTarget);
		////mainObjPos += distance * deltaTarget;
		//mainObjPos = distance * deltaTarget;
		//distance = glm::length(mainObjPos - camera->Position);

		//lastTarget = currentTarget;
		//currentTarget = camera->Position + camera->Front;

		return;
	}

}
//void callback_Char(GLFWwindow* window, unsigned int key)
//{
//	keyToggles[key] = !keyToggles[key];
//}

// If the window is resized, capture the new size and reset the viewport
void callback_WindowSize(GLFWwindow* window, int width, int height) {
	g_displayWidth = width;
	g_displayHeight = height;

	//glViewport(0, 0, width, height);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void callback_Scroll(GLFWwindow* window, double xoffset, double yoffset) {
	camera->ProcessMouseScroll(yoffset);
}


// This function is called once to initialize the scene and OpenGL
static void init() {
	// Initialize time.
	glfwSetTime(0.0);

	camera = new Camera{ glm::vec3{0.0f, 1.0f, 10.0f} };
	camera->Front = glm::vec3{ 0.0f, 0.0f, -1.0f };  // adjust viewDirectionVec direction; may reverse keys W and S
	distance = glm::length(mainObjPos - camera->Position);
	currentTarget = lastTarget = camera->Position + camera->Front;

	camera->Zoom = 60.0f; // initial for cubemap

	// load pbrmaterial  textures
	for (int i = 0; i < numObjects; i++) {
		for (int j = 0; j < 5; j++) {
			pbrTextures[i][j] = loadTexture(get_commonResources(pbfTexFile[i][j]).c_str(), false); // flip: false
		}
	}

	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);
	// set depth function to less than AND equal for skybox depth trick.
	glDepthFunc(GL_LEQUAL);
	// enable seamless cubemap sampling for lower mip levels in the pre-filter map.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Buffer generation for tessellation only
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	genMeshCube(meshPatches, normalLines, bendNormal);
	bindBufferPatches(meshPatches, VAO, VBO, EBO);

	glGenVertexArrays(1, &LVAO);
	glGenBuffers(1, &LVBO);
	glGenBuffers(1, &LEBO);
	genMeshAxes(axesMesh);
	bindBufferLines(axesMesh, LVAO, LVBO, LEBO);

	progImageToCubemap = new MyShader::Shader{ get_shader("01_cubemap.vert").c_str(), get_shader("01_cubemap.frag").c_str() };
	//progEnvCubemap = new MyShader::Shader{ get_shader_root("01_cubemap.vert").c_str(), get_shader_root("01_cubemap.frag").c_str() };

	progAxes = new MyShader::Shader{ get_shader("01_axes.vert").c_str(), get_shader("01_axes.frag").c_str() };

	progBG = new MyShader::Shader{ get_shader("01_background.vert").c_str(), get_shader("01_background.frag").c_str() };
	progBG_tes = new MyShader::Shader{ get_shader("01_background_tes.vert").c_str(), get_shader("01_background_tes.frag").c_str(), get_shader("01_background_tes.tesc").c_str(), get_shader("01_background_tes.tese").c_str(), get_shader("01_background_tes.geom").c_str() };

	progRefract = new MyShader::Shader{ get_shader("01_refract.vert").c_str(), get_shader("01_refract.frag").c_str() };
	progPBR = new MyShader::Shader{ get_shader("01_pbr.vert").c_str(), get_shader("01_pbr.frag").c_str() };
	progIrradiance = new MyShader::Shader{ get_shader("01_cubemap.vert").c_str(), get_shader("01_irradiance_convolution.frag").c_str() };
	progPrefilter = new MyShader::Shader{ get_shader("01_cubemap.vert").c_str(), get_shader("01_prefilter.frag").c_str() };
	progBRDF = new MyShader::Shader{ get_shader("01_brdf.vert").c_str(), get_shader("01_brdf.frag").c_str() };

	progImageToCubemap->use();
	progImageToCubemap->setInt("background", 0);
	progImageToCubemap->setInt("reflection", 1);
	progImageToCubemap->setInt("environment", 2);

	progBG->use();
	progBG->setInt("environmentMap", 0);
	progBG_tes->use();
	progBG_tes->setInt("environmentMap", 0);

	progRefract->use();
	progRefract->setInt("captureCubemap", 0);

	progPBR->use();
	progPBR->setInt("irradianceMap", 0);
	progPBR->setInt("prefilterMap", 1);
	progPBR->setInt("brdfLUT", 2);
	progPBR->setInt("albedoMap", 3);
	progPBR->setInt("normalMap", 4);
	progPBR->setInt("metallicMap", 5);
	progPBR->setInt("roughnessMap", 6);
	progPBR->setInt("aoMap", 7);

	progIrradiance->use();
	progIrradiance->setInt("environmentMap", 0);

	progPrefilter->use();
	progPrefilter->setInt("environmentMap", 0);

	sphere = new MyModel::Model{ get_commonResources("objects/sphere/sphere_blender.obj") };
	//sphere = new MyModel::Model{ get_globaldata_root("objects/sphere/sphere.obj") };

	//cube = new MyModel::Model{ get_globaldata_root("objects/cube/cube.obj") };
	cube = new MyModel::Model{ get_commonResources("objects/cube/cube_blender.obj") };

	/* pbr: setup framebuffer*/
	//1 capture FBO
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, g_captureFboWidth, g_captureFboHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//2 irradiance FBO
	glGenFramebuffers(1, &irradianceFBO);
	glGenRenderbuffers(1, &irradianceRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, irradianceFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, irradianceRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, g_irradianceFboWidth, g_irradianceFboHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, irradianceRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//3 prefilter FBO
	glGenFramebuffers(1, &prefilterFBO);
	glGenRenderbuffers(1, &prefilterRBO);

	//4 BRDF lookup table FBO
	glGenFramebuffers(1, &brdfLutFBO);
	glGenRenderbuffers(1, &brdfLutRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, brdfLutFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, brdfLutRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, g_brdfLutFBOWidth, g_brdfLutFBOHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, brdfLutRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// set up capture textures in framebuffer
	genCaptureCubemaps();

	// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
	//setupIrradianceCubemap();
	genIrradianceCubemaps();

	// pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale. does not work
	genPrefilterCubemaps();
	//GLSL::checkError(GET_FILE_LINE);

	// pbr: generate a 2D LUT from the BRDF equations used.
	genBrdfLutTexture();
	updateBrdfLutTexture();

	// finally 
	genEnvCubemap();

}

void renderOnFB(int objectid, bool pbrswitch, bool reflectanceswitch) {

	glViewport(0, 0, g_captureFboWidth, g_captureFboHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //??
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

	P = captureProjection(90.0f, nearClipPlane, farClipPlaneByObject[objectid]);

	for (int face = 0; face < 6; face++) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, captureCubemaps[objectid], 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		V = getCaptureViewMat(face, objectPos[objectid]);

		for (int k = 0; k < numObjects; ++k) {  // draw each sphere
			if (k == objectid) {
				// do not draw for itself
				continue;
			}

			if ((k == 0) & (refractionSwitch)) {
				progRefract->use();
				progRefract->setMat4("projection", P);
				progRefract->setMat4("view", V);
				M = glm::mat4{ 1.0f };
				M = glm::translate(M, objectPos[k]);
				M = glm::scale(M, objectScale[k]);;
				progRefract->setMat4("model", M);
				progRefract->setBool("reflectanceSwitch", reflectanceswitch);
				progRefract->setVec3("cameraPos", objectPos[k]);
				progRefract->setVec3("objectPos", objectPos[k]);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, captureCubemaps[k]);
				sphere->Draw(progRefract);
				continue;
			}

			progPBR->use();
			progPBR->setBool("pbrSwitch", pbrswitch);
			progPBR->setMat4("projection", P);
			progPBR->setMat4("view", V);
			progPBR->setVec3("cameraPos", objectPos[objectid]);

			if (pbrswitch) {// PBR
				//1  irradiance
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceCubemaps[k]);
				//2prefilter
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterCubemaps[k]);
				//3 brdf (fixed)
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, brdfLutTexture);
			}
			else {// no PBR
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_CUBE_MAP, captureCubemaps[k]);
			}

			M = glm::mat4{ 1.0f };
			M = glm::translate(M, objectPos[k]);
			M = glm::scale(M, objectScale[k]);
			progPBR->setMat4("model", M);
			for (int l = 0; l < 5; l++) {
				glActiveTexture(GL_TEXTURE3 + l);
				glBindTexture(GL_TEXTURE_2D, pbrTextures[k][l]);
			}
			sphere->Draw(progPBR);
		}


		// render skybox (render as last to prevent overdraw)
		if (tessellationSwitch) {
			progBG_tes->use();
			progBG_tes->setMat4("projection", P);
			progBG_tes->setMat4("view", V);
			progBG_tes->setFloat("gTessellationLevel", tessellationLevel);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
			drawElements(VAO, VBO, EBO, GL_PATCHES, (GLuint)meshPatches.indices.size(), &meshPatches.indices[0]);
		}
		else {
			progBG->use();
			progBG->setMat4("projection", P);
			progBG->setMat4("view", V);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
			//renderCubeMap(cubeMapVAO, cubeMapVBO);
			cube->Draw(progBG);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)??
	glBindTexture(GL_TEXTURE_CUBE_MAP, captureCubemaps[objectid]);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

}

void renderAllOnScreen(bool pbrswitch, bool reflectanceswitch) {

	//Finally Render to screen  - Redraw all objects to the screen
	glViewport(0, 0, g_displayWidth, g_displayHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	P = glm::perspective(glm::radians(camera->Zoom), (float)g_displayWidth / (float)g_displayHeight, 0.1f, 800.0f);
	V = camera->GetViewMatrix();
	cameraPos = camera->Position; // camera position in world coords is in the last column

	for (int i = 0; i < numObjects; i++) {
		if ((i == 0) & (refractionSwitch)) {
			progRefract->use();
			progRefract->setMat4("projection", P);
			progRefract->setMat4("view", V);
			M = glm::mat4{ 1.0f };
			M = glm::translate(M, objectPos[i]);
			M = glm::scale(M, objectScale[i]);;
			progRefract->setMat4("model", M);
			progRefract->setVec3("cameraPos", cameraPos);
			progRefract->setVec3("objectPos", objectPos[i]);
			progRefract->setBool("reflectanceSwitch", reflectanceswitch);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, captureCubemaps[i]);
			sphere->Draw(progRefract);
			glUseProgram(0);
			continue;
		}

		progPBR->use();
		progPBR->setBool("pbrSwitch", pbrswitch);
		progPBR->setMat4("projection", P);
		progPBR->setMat4("view", V);
		M = glm::mat4{ 1.0f };
		M = glm::translate(M, objectPos[i]);
		M = glm::scale(M, objectScale[i]);
		progPBR->setMat4("model", M);
		progPBR->setVec3("cameraPos", cameraPos);
		if (pbrswitch) {// PBR: Note may cause poor image rendering
			//1  irradiance
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceCubemaps[i]);
			//2prefilter
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterCubemaps[i]);
			//3 brdf (fixed)
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, brdfLutTexture);
		}
		else {// no PBR
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, captureCubemaps[i]);
		}

		for (int l = 0; l < 5; l++) {
			glActiveTexture(GL_TEXTURE3 + l);
			glBindTexture(GL_TEXTURE_2D, pbrTextures[i][l]);
		}
		sphere->Draw(progPBR);

		glUseProgram(0);
	}

	// Draw axes
	progAxes->use();
	progAxes->setMat4("projection", P);
	progAxes->setMat4("view", V);
	M = glm::mat4{ 1.0f };
	M = glm::scale(M, glm::vec3{9.0f});
	progAxes->setMat4("model", M);
	std::vector<GLuint> Indices = axesMesh.indices;
	//GLenum bind_Mode = LineBuff;
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(2.0f);
	drawElements(LVAO, LVBO, LEBO, GL_LINES, (GLuint)Indices.size(), &Indices[0]);
	glLineWidth(1.0f);
	glDisable(GL_LINE_SMOOTH);
	glUseProgram(0);

	// render skybox (render as last to prevent overdraw)
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(2.0f);
	if (tessellationSwitch) {
		progBG_tes->use();
		progBG_tes->setMat4("projection", P);
		progBG_tes->setMat4("view", V);
		progBG_tes->setFloat("gTessellationLevel", tessellationLevel);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		drawElements(VAO, VBO, EBO, GL_PATCHES, (GLuint)meshPatches.indices.size(), &meshPatches.indices[0]);

		glUseProgram(0);
	}
	else {
		progBG->use();
		progBG->setMat4("projection", P);
		progBG->setMat4("view", V);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		//renderCubeMap(cubeMapVAO, cubeMapVBO);
		cube->Draw(progBG);

		glUseProgram(0);
	}

	glLineWidth(1.0f);
	glDisable(GL_LINE_SMOOTH);

}

void genCaptureCubemaps() {

	glGenTextures(arrayMapSize, &captureCubemaps[0]);
	for (int i = 0; i < arrayMapSize; ++i) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, captureCubemaps[i]);
		for (unsigned int face = 0; face < 6; ++face) {
			//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB16F, g_captureFboWidth, g_captureFboHeight, 0, GL_RGB, GL_FLOAT, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB, g_captureFboWidth, g_captureFboHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

}


void genIrradianceCubemaps() {

	// multiple textures
	glGenTextures(arrayMapSize, &irradianceCubemaps[0]);
	for (int i = 0; i < arrayMapSize; i++) {
		//glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceCubemaps[i]);
		for (unsigned int face = 0; face < 6; ++face) {
			//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB16F, g_irradianceFboWidth, g_irradianceFboHeight, 0, GL_RGB, GL_FLOAT, nullptr);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB, g_irradianceFboWidth, g_irradianceFboHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	}
}


void updateIrradianceCubemaps(int objectid) {

	glViewport(0, 0, g_irradianceFboWidth, g_irradianceFboWidth); // don't forget to configure the viewport to the capture dimensions.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	progIrradiance->use();
	//progIrradiance->setMat4("projection", captureProjection(90.0f, farClipPlane[objectid]));
	progIrradiance->setMat4("projection", captureProjection(90.0f, nearClipPlane, farClipPlane));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, captureCubemaps[objectid]);

	glBindFramebuffer(GL_FRAMEBUFFER, irradianceFBO);

	for (unsigned int face = 0; face < 6; ++face) {
		progIrradiance->setMat4("view", getCaptureViewMat(face, captureViewPara[face].eyePos));
		//GLSL::checkError(GET_FILE_LINE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, irradianceCubemaps[objectid], 0);
		//GLSL::checkError(GET_FILE_LINE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//renderCubeMap(cubeMapVAO, cubeMapVBO);
		cube->Draw(progIrradiance);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


void genPrefilterCubemaps() {
	// pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.

	glGenTextures(arrayMapSize, &prefilterCubemaps[0]);
	for (int i = 0; i < arrayMapSize; i++) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterCubemaps[i]);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0); // new
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 4); // new
		for (unsigned int face = 0; face < 6; ++face) {
			//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB16F, g_prefilterFboWidth, g_prefilterFboHeight, 0, GL_RGB, GL_FLOAT, nullptr);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB, g_prefilterFboWidth, g_prefilterFboHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minifcation filter to mip_linear 
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		/*GLfloat lod;
		glGetTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, &lod);
		std::cout << i << " prefilter LOD base level:  " << lod << std::endl;
		glGetTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, &lod);
		std::cout << i << " prefilter LOD max lvele:  " << lod << std::endl;*/

	}

}


void updatePrefilterCubemaps(int objectid) {
	// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
	progPrefilter->use();
	//progPrefilter->setInt("environmentMap", 0);
	//progPrefilter->setMat4("projection", captureProjection(90.0f, farClipPlane[objectid]));
	progPrefilter->setMat4("projection", captureProjection(90.0f, nearClipPlane, farClipPlane));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, captureCubemaps[objectid]);

	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = g_prefilterFboWidth * std::pow(0.5, mip);
		unsigned int mipHeight = g_prefilterFboHeight * std::pow(0.5, mip);
		glViewport(0, 0, mipWidth, mipHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, prefilterFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, prefilterRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, prefilterRBO);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		progPrefilter->setFloat("roughness", roughness);
		for (unsigned int face = 0; face < 6; ++face) {
			//progPrefilter->setMat4("view", getCaptureViewMat(face, objectPos[objectid]));
			progPrefilter->setMat4("view", getCaptureViewMat(face, captureViewPara[face].eyePos));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, prefilterCubemaps[objectid], mip);
			// check FBO status

			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE) {
				std::cout << "GL_FRAMEBUFFER Status: " << status << std::endl;
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//renderCubeMap(cubeMapVAO, cubeMapVBO);
			cube->Draw(progPrefilter);
			//GLSL::checkError(GET_FILE_LINE);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // moved here
	}

}


void genBrdfLutTexture() {

	glBindFramebuffer(GL_FRAMEBUFFER, brdfLutFBO);
	glGenTextures(1, &brdfLutTexture);
	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, brdfLutTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, g_brdfLutFBOWidth, g_brdfLutFBOHeight, 0, GL_RG, GL_FLOAT, 0);

	// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}

void updateBrdfLutTexture() {
	// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.

	glBindFramebuffer(GL_FRAMEBUFFER, brdfLutFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLutTexture, 0);

	glViewport(0, 0, g_brdfLutFBOWidth, g_brdfLutFBOHeight);
	progBRDF->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderQuad(quadVAO, quadVBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void genEnvCubemap() {

	for (int i = 0; i < 3; i++) {
		iblFile[i].textureid = loadTexture(get_commonResources(iblFile[i].filename).c_str(), true);
	}

	// pbr: setup cubemap to render to and attach to framebuffer
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int face = 0; face < 6; ++face) {
		//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB16F, g_captureFboWidth, g_captureFboHeight, 0, GL_RGB, GL_FLOAT, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB, g_captureFboWidth, g_captureFboHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// pbr: convert HDR equirectangular environment map to cubemap equivalent

	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	progImageToCubemap->use();
	progImageToCubemap->setMat4("projection", captureProjection(90.0f, nearClipPlane, farClipPlane));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, iblFile[0].textureid);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, iblFile[1].textureid);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, iblFile[2].textureid);
	glViewport(0, 0, g_captureFboWidth, g_captureFboHeight); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int face = 0; face < 6; ++face) {
		progImageToCubemap->setMat4("view", getCaptureViewMat(face, captureViewPara[face].eyePos));
		progImageToCubemap->setVec3("cameraPos", captureViewPara[face].eyePos);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//renderCubeMap(cubeMapVAO, cubeMapVBO);
		cube->Draw(progImageToCubemap);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	//writeCubemap_png(envCubemap, g_captureFboWidth, g_captureFboHeight, "other/envCubemap");
}
