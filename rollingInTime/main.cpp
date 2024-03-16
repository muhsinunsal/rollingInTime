#define STB_IMAGE_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "ClockPlatform.h"
#include "Player.h"

#include "Model.h"

#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecularIntensity = 0, uniformShininess = 0,
uniformDirectionalLightTransform = 0, uniformOmniLightPos = 0, uniformFarPlane = 0;

Window mainWindow;
std::vector<Mesh*> meshList;

std::vector<Shader> shaderList;
Shader directionalShadowShader;
Shader omniShadowShader;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture startScreenTexture;

Model startScreen;
Model finalScreen;

Model point1;
Model point2;
Model point3;
Model point4;
Model point5;
Model point6;

Material shinyMaterial;
Material dullMaterial;


DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

Skybox skybox;

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

GLboolean isGameRunning = false;

GLfloat gameStartTime = 0.0f;

ClockPlatform clockPlatform;
Player player;


void jumpToGame() {
	camera.position = glm::vec3(-25.0f, 10.0f, 25.0f);
	camera.yaw = 0;
	camera.pitch = -20.0f;
	gameStartTime = glfwGetTime();
}
void jumpToEnd() {
	player.health = 1.0f;
	camera.position = glm::vec3(-260.0f, 60.0f, -31.0f);
	camera.yaw = -180.0f;
	camera.pitch = 0.0f;
}

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{

	GLfloat vertices[] = {
		0.75f,  0.65f, 0.0f,  1.0f, 1.0f,        0.0f, 0.0f, 1.0f,
		0.75f, -0.65f, 0.0f,  1.0f, 0.0f,        0.0f, 0.0f, 1.0f,
	   -0.75f, -0.65f, 0.0f,  0.0f, 0.0f,        0.0f, 0.0f, 1.0f,
	   -0.75f,  0.65f, 0.0f,  0.0f, 1.0f,        0.0f, 0.0f, 1.0f
	};

	GLuint indices[] = {
		0, 1, 3,
		1, 2, 3
	};



	//calcAverageNormals(indices, 12, vertices, 32, 8, 5);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
	omniShadowShader.CreateFromFiles("Shaders/omni_shadow_map.vert", "Shaders/omni_shadow_map.geom", "Shaders/omni_shadow_map.frag");
}

void RenderScene()
{
	glm::mat4 model(1.0f);

	model = glm::translate(model, glm::vec3(-90.0f, 50.0f, -70.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-17.5f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	startScreen.RenderModel();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-270.0f, 60.0f, -31.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	finalScreen.RenderModel();

	player.render(uniformModel, deltaTime);

	clockPlatform.render(&player, uniformModel, deltaTime);


	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-270.0f, 58.0f, -30.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	if (player.point > 60) {
		point6.RenderModel();
	}
	else if (player.point > 40) {
		point5.RenderModel();
	}
	else if (player.point > 30) {
		point4.RenderModel();
	}
	else if (player.point > 20) {
		point3.RenderModel();
	}
	else if (player.point > 10) {
		point2.RenderModel();
	}
	else {
		point1.RenderModel();
	}

	//printf("Time: %f\n",glfwGetTime()- gameStartTime);
	if (isGameRunning) {
		clockPlatform.updateHourSpeed(0.125);
		clockPlatform.updateMinuteSpeed(0.37f * glm::pow(glm::e<float>(), (glfwGetTime() - gameStartTime) / 50));
		clockPlatform.updateSecondSpeed(0.73f * glm::pow(glm::e<float>(), (glfwGetTime() - gameStartTime) / 50));
	}

	if (player.health == 0.0f) {
		isGameRunning = false;
		clockPlatform.updateMinuteSpeed(0.0f);
		clockPlatform.updateSecondSpeed(0.0f);
		clockPlatform.minuteAngle = glm::radians(0.0f);
		clockPlatform.secondAngle = glm::radians(0.0f);

		jumpToEnd();
	}


}

void DirectionalShadowMapPass(DirectionalLight* light)
{
	directionalShadowShader.UseShader();

	glViewport(0, 0, light->getShadowMap()->GetShadowWidth(), light->getShadowMap()->GetShadowHeight());

	light->getShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();
	//directionalShadowShader.SetDirectionalLightTransform(&light->CalculateLightTransform());

	directionalShadowShader.Validate();

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadowMapPass(PointLight* light)
{
	omniShadowShader.UseShader();

	glViewport(0, 0, light->getShadowMap()->GetShadowWidth(), light->getShadowMap()->GetShadowHeight());

	light->getShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = omniShadowShader.GetModelLocation();
	uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
	uniformFarPlane = omniShadowShader.GetFarPlaneLocation();

	glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
	glUniform1f(uniformFarPlane, light->GetFarPlane());
	omniShadowShader.SetLightMatrices(light->CalculateLightTransform());

	omniShadowShader.Validate();

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	glViewport(0, 0, 1366, 768);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skybox.DrawSkybox(viewMatrix, projectionMatrix);

	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformModel = shaderList[0].GetModelLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	shaderList[0].SetDirectionalLight(&mainLight);
	shaderList[0].SetPointLights(pointLights, pointLightCount, 3, 0);
	shaderList[0].SetSpotLights(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);
	//shaderList[0].SetDirectionalLightTransform(&mainLight.CalculateLightTransform());

	mainLight.getShadowMap()->Read(GL_TEXTURE2);
	shaderList[0].SetTexture(1);
	shaderList[0].SetDirectionalShadowMap(2);

	glm::vec3 lowerLight = camera.getCameraPosition();
	lowerLight.y -= 0.3f;

	shaderList[0].Validate();



	RenderScene();
}

void handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			player.rotateControl(key, isGameRunning);

			if (key == GLFW_KEY_ENTER) {
				jumpToGame();
				isGameRunning = true;
			}


			theWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
		}
	}
}

void handleMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	glfwSetKeyCallback(mainWindow.mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow.mainWindow, handleMouse);

	CreateObjects();
	CreateShaders();


	camera = Camera(glm::vec3(-114.0f, 57.0f, -57.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, -17.5f, 13.5f, 0.5f);

	startScreen = Model();
	startScreen.LoadModel("Models/startingPlane.dae");

	finalScreen = Model();
	finalScreen.LoadModel("Models/finalPlane.dae");

	point1 = Model();
	point1.LoadModel("Models/point1.dae");

	point2 = Model();
	point2.LoadModel("Models/point2.dae");

	point3 = Model();
	point3.LoadModel("Models/point3.dae");

	point4 = Model();
	point4.LoadModel("Models/point4.dae");

	point5 = Model();
	point5.LoadModel("Models/point5.dae");

	point6 = Model();
	point6.LoadModel("Models/point6.dae");



	shinyMaterial = Material(4.0f, 256);
	dullMaterial = Material(0.3f, 4);

	clockPlatform = ClockPlatform(2);
	player = Player(glm::vec3(0.0f, 0.2f, 25.0f));


	mainLight = DirectionalLight(2048, 2048,
		1.0f, 0.53f, 0.3f,
		0.1f, 0.9f,
		15.0f, -12.0f, 18.5);


	spotLights[0] = SpotLight(1024, 1024,
		0.01f, 100.0f,
		1.0f, 1.0f, 1.0f,
		0.2f, 0.1f,
		0.0f, 50.0f, 25.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	spotLights[1] = SpotLight(1024, 1024,
		0.01f, 100.0f,
		1.0f, 1.0f, 1.0f,
		0.2f, 0.1f,
		-260.0f, 60.0f, -31.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 200.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1);


	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;


		//printf("%.3f\t%.3f\t%.3f\t\t%.3f\t%.3f\n", camera.position.x, camera.position.y, camera.position.z, camera.yaw, camera.pitch);
		//printf("[POINT] %d\n", player.point);
		// Get + Handle User Input
		glfwPollEvents();

		player.moveControl(mainWindow.getsKeys(), deltaTime);

		//camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		if (mainWindow.getsKeys()[GLFW_KEY_L])
		{
			spotLights[0].Toggle();
			mainWindow.getsKeys()[GLFW_KEY_L] = false;
		}

		DirectionalShadowMapPass(&mainLight);
		for (size_t i = 0; i < pointLightCount; i++)
		{
			OmniShadowMapPass(&pointLights[i]);
		}
		for (size_t i = 0; i < spotLightCount; i++)
		{
			OmniShadowMapPass(&spotLights[i]);
		}
		RenderPass(camera.calculateViewMatrix(), projection);

		mainWindow.swapBuffers();
	}
	return 0;
}