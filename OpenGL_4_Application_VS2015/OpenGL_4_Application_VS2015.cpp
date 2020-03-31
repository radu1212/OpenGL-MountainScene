//
//  main.cpp
//  OpenGL Shadows
//
//  Created by CGIS on 05/12/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "SkyBox.hpp"

#include "Camera.hpp"
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"

int glWindowWidth = 640;
int glWindowHeight = 480;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

std::vector<const GLchar*> faces;
gps::SkyBox mySkyBox;

GLfloat lastX = 320, lastY = 240;
bool firstMouse = true;

GLfloat yaw, pitch;

const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(glm::vec3(0.0f, 3.5f, 12.0f), glm::vec3(0.0f, 0.0f, 0.0f));
GLfloat cameraSpeed = 0.03f;

bool pressedKeys[1024];
GLfloat angle;
GLfloat lightAngle;

gps::Model3D myModel;
gps::Model3D ground;
gps::Model3D lightCube;
gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader depthMapShader;
gps::Shader skyboxShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;
GLuint time = 0;
GLuint texture = 1;

gps::Model3D man1;
GLfloat businessMan = 0.1;
bool left = true;
float x = 0;
float y = 0;

float angle1 = -90;

gps::Model3D car1;
GLfloat truck = 0.1;
bool roadEnd = false;


gps::Model3D car2;
GLfloat race = 0.1;
GLfloat shift = 0;
bool roadEnd2 = false;
bool startTurn = false;

bool sync = false;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	lightShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) {
		lastX = 320;
		lastY = 240;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	float sensitivity = 0.3;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	myCamera.rotate(pitch, yaw);
}

void scenePresentation() {
	if (time < 150) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		myCamera.rotate(-18, -90);
		printf("Hi there!!!\n");
	}

	if (time > 220   && time < 270) {
		
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		myCamera.rotate(-18 - x, -90);
		x += 0.3;
		printf("Welcome to Radu`s creepy city!!!\n");

	}

	if (time > 270 && x > -18) {

		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		myCamera.rotate(-18 - x, -90);
		x -= 0.3;
		printf("This road is older than Route 66, trust me\n");

	}

	if (time > 670 && time < 780) {

		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		myCamera.rotate(0, -90);
		printf("Careful at this traffic, it`s very dangerous!\n");
	
	}

	if (time > 780 && time < 1050) {

		//myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		myCamera.rotate(0, angle1);
		angle1++;
		printf("OMG, this really is creepy AF\n");

	}

	if (time > 1050 && time < 1090) {

		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		myCamera.rotate(0, angle1);
		printf("The salesman is looking to buy a new haunted house, he`s stupid!\n");
	}

	if (time > 1300 && angle1 < 270) {

		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed/2);
		myCamera.rotate(0, angle1);
		angle1++;
		printf("Let`s get tf outta here!!\n");

	}

	if (time > 1450 && time < 1730) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		myCamera.rotate(0, angle1);
		printf("RUUUUUUN!!!!!\n");
	}

	if (time > 1730 && angle1 < 450) {

		//myCamera.move(gps::MOVE_BACKWARD, cameraSpeed / 2);
		myCamera.rotate(0, angle1);
		angle1++;
		printf("We made it\n");
	}

	if (time > 1950 && time < 2060) {

		myCamera.move(gps::MOVE_FORWARD, -cameraSpeed);
		myCamera.rotate(y, 90);
		y -= 0.5;
		printf("Bye bye horror city\n");

	}
	if (time > 2060 && time < 2140) {

		myCamera.move(gps::MOVE_FORWARD, -cameraSpeed);
		myCamera.rotate(y, 90);
		y += 0.5;
		printf("Hope I`ll never end up here again!\n");

	}

	if (time > 2140 && time < 2710) {
		lightAngle -= 0.9f;
		if (lightAngle < 0.0f)
			lightAngle += 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	if (time > 2710 && time < 2850) {
		texture = 0;
	}


	if (time > 2850 && time < 2990) {
		texture = 2;
	}


	if (time > 2990 && time < 3000) {
		texture = 1;
	}
}

void processMovement()
{

	if (pressedKeys[GLFW_KEY_Q]) {
		angle += 0.1f;
		if (angle > 360.0f)
			angle -= 360.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle -= 0.1f;
		if (angle < 0.0f)
			angle += 360.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_J]) {

		lightAngle += 0.3f;
		if (lightAngle > 360.0f)
			lightAngle -= 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle -= 0.3f;
		if (lightAngle < 0.0f)
			lightAngle += 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	if (pressedKeys[GLFW_KEY_Z]) {
		texture = 0;
	}

	if (pressedKeys[GLFW_KEY_X]) {
		texture = 1;
	}

	if (pressedKeys[GLFW_KEY_C]) {
		texture = 2;
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
						  //glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initFBOs()
{
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix()
{
	const GLfloat near_plane = 1.0f, far_plane = 10.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(lightDirTr, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

void initModels()
{
	myModel = gps::Model3D("objects/gps/v19.obj", "objects/gps/");
	car1 = gps::Model3D("objects/gps/e100.obj", "objects/gps/");
	car2 = gps::Model3D("objects/gps/e200.obj", "objects/gps/");
	man1 = gps::Model3D("objects/gps/GTP_BMan_Jack_07_Stg_Lsn_Adl_Ccs_Gry_Mgr.obj", "objects/gps/");
	ground = gps::Model3D("objects/ground/ground.obj", "objects/ground/");
	lightCube = gps::Model3D("objects/cube/cube.obj", "objects/cube/");


	faces.push_back("skybox/right.tga");
	faces.push_back("skybox/left.tga");
	faces.push_back("skybox/top.tga");
	faces.push_back("skybox/bottom.tga");
	faces.push_back("skybox/back.tga");
	faces.push_back("skybox/front.tga");
}

void initShaders()
{
	mySkyBox.Load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();

	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");
}

void initUniforms()
{
	myCustomShader.useShaderProgram();

	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");

	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");

	lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 2.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	mySkyBox.Load(faces);

	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void renderScene()
{

	if (texture == 0) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (texture == 1)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (texture == 2)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	processMovement();
	scenePresentation();
	time++;
	//printf("%d \n", time);

	//render the scene to the depth buffer (first pass)

	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	//create model matrix for nanosuit
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));

	myModel.Draw(depthMapShader);

	//create model matrix for man
	model = glm::mat4(1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f + businessMan, 0.0f, 0.0f));
	if (businessMan < 0) {
		left = true;
	}
	if (businessMan > 1.837) {
		left = false;
	}
	if (left) businessMan += 0.005;
	else businessMan -= 0.005;
	
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));

	man1.Draw(depthMapShader);


	//create model matrix for man
	model = glm::mat4(1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f , 0.0f, 0.0f + truck));
	if (truck < 0) {
		roadEnd = false;
	}
	if (truck > 16) {
		roadEnd = true;
	}
	if (roadEnd) {
		model = glm::mat4(1.0f);
		truck = 0;
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); 
		roadEnd = false;
		sync = true;
	}
	truck += 0.02;
	//printf("%f \n", truck);

	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));

	car1.Draw(depthMapShader);


	//create model matrix for man
	model = glm::mat4(1.0f);
	if (race > 4.5 && race < 10) startTurn = true;
	if (race > 10) startTurn = false;
	if (!startTurn && race < 10) model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f + race));
	else if (!startTurn && race > 10) model = glm::translate(glm::mat4(1.0f), glm::vec3(-shift, 0.0f, 0.0f + race));
	else {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f - shift, 0.0f, 0.0f + race));
		shift += 0.003;
	}
	if (race < 0) {
		roadEnd2 = false;
	}
	if (race > 16) {
		roadEnd2 = true;
		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10000.0f, 0.0f));
	}
	if (roadEnd2 && sync) {
		model = glm::mat4(1.0f);
		race = 0;
		shift = 0;
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); 
		roadEnd2 = false;
		sync = false;
	}

	race += 0.03;
	//printf("%f \n", race);

	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));

	car2.Draw(depthMapShader);
	
	
	//create model matrix for ground
	model = glm::mat4(1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));

	ground.Draw(depthMapShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//render the scene (second pass)

	myCustomShader.useShaderProgram();

	//send lightSpace matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	//send view matrix to shader
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"),
		1,
		GL_FALSE,
		glm::value_ptr(view));

	//compute light direction transformation matrix
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	//send lightDir matrix data to shader
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	glViewport(0, 0, retina_width, retina_height);
	myCustomShader.useShaderProgram();

	//bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

	//create model matrix for nanosuit
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	myModel.Draw(myCustomShader);

	//create model matrix for businessman
	model = glm::mat4(1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f + businessMan, 0.0f, 0.0f));
	if (businessMan < 0) {
		left = true;
	}
	if (businessMan > 1.837) {
		left = false;
	}
	if (left) businessMan += 0.005;
	else businessMan -= 0.005;
	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	man1.Draw(myCustomShader);

	//create model matrix for nanosuit
	model = glm::mat4(1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f + truck)); 
	if (truck < 0) {
		roadEnd = false;
	}
	if (truck > 16) {
		roadEnd = true;
	}
	if (roadEnd) {
		model = glm::mat4(1.0f);
		truck = 0;
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); 
		roadEnd = false;
		sync = true;
	}
	truck += 0.02;
	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	car1.Draw(myCustomShader);

	//create model matrix for nanosuit
	model = glm::mat4(1.0f);
	if (race > 4.5 && race < 10) startTurn = true;
	if (race > 10) startTurn = false;
	if (!startTurn && race < 10) model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f + race)); 
	else if (!startTurn && race > 10) model = glm::translate(glm::mat4(1.0f), glm::vec3(-shift, 0.0f, 0.0f + race));
	else {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f - shift, 0.0f, 0.0f + race));
		shift += 0.003;
	}
	if (race < 0) {
		roadEnd2 = false;
	}
	if (race > 16) {
		roadEnd2 = true;
		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10000.0f, 0.0f));
	}
	if (roadEnd2 && sync) {
		model = glm::mat4(1.0f);
		race = 0;
		shift = 0;
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		roadEnd2 = false;
		sync = false;
	}
	
	race += 0.03;
	//printf("%f \n", race);
	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	car2.Draw(myCustomShader);
	
	//create model matrix for ground
	//send model matrix data to shader
	model = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	ground.Draw(myCustomShader);

	//draw a white cube around the light

	lightShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	model = glm::mat4(1.0f);
	model = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(2*lightDir.x, 4, 2 * lightDir.z));
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	lightCube.Draw(lightShader);

	mySkyBox.Draw(skyboxShader, view, projection);

}

int main(int argc, const char * argv[]) {

	initOpenGLWindow();
	initOpenGLState();
	initFBOs();
	initModels();
	initShaders();
	initUniforms();
	glCheckError();
	while (!glfwWindowShouldClose(glWindow)) {
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
