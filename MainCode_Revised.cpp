///////////////////////////////////////////////////////////////////////////////
// maincode.cpp
// ============
// Entry point of the application. Initializes GLEW, GLFW, and sets up rendering.
// Enhanced with error handling, logging, modularity, and resource management.
//
// AUTHOR: Brian Battersby - Modified by Asia for CS-330 Final
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>

#include <GL/glew.h>
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SceneManager.h"
#include "ViewManager.h"
#include "ShapeMeshes.h"
#include "ShaderManager.h"

// Global Variables
namespace {
	const char* const WINDOW_TITLE = "7-1 FinalProject and Milestones";
	GLFWwindow* g_Window = nullptr;
	SceneManager* g_SceneManager = nullptr;
	ShaderManager* g_ShaderManager = nullptr;
	ViewManager* g_ViewManager = nullptr;
	std::ofstream logFile;
}

// Function Declarations
bool InitializeGLFW();
bool InitializeGLEW();
void InitializeManagers();
void RenderLoop();
void Cleanup();
void Log(const std::string& message);
bool CheckGLError(const std::string& context);

/***********************************************************
 *  main()
 ***********************************************************/
int main(int argc, char* argv[]) {
	logFile.open("debug_log.txt", std::ios::out);
	Log("Application started");

	if (!InitializeGLFW()) {
		Log("GLFW initialization failed");
		return EXIT_FAILURE;
	}

	InitializeManagers();

	if (!g_Window || !InitializeGLEW()) {
		Log("GLEW initialization failed");
		Cleanup();
		return EXIT_FAILURE;
	}

	g_ShaderManager->LoadShaders("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl");
	g_ShaderManager->use();
	CheckGLError("Shader Loading");

	g_SceneManager = new SceneManager(g_ShaderManager);
	g_SceneManager->PrepareScene();
	CheckGLError("Scene Preparation");

	RenderLoop();
	Cleanup();
	Log("Application terminated");
	logFile.close();
	return EXIT_SUCCESS;
}

/***********************************************************
 * InitializeGLFW()
 ***********************************************************/
bool InitializeGLFW() {
	if (!glfwInit()) {
		Log("GLFW Init failed");
		return false;
	}
#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	return true;
}

/***********************************************************
 * InitializeGLEW()
 ***********************************************************/
bool InitializeGLEW() {
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		Log("GLEW Error: " + std::string((const char*)glewGetErrorString(err)));
		return false;
	}
	std::cout << "OpenGL Initialized\nVersion: " << glGetString(GL_VERSION) << std::endl;
	return true;
}

/***********************************************************
 * InitializeManagers()
 ***********************************************************/
void InitializeManagers() {
	g_ShaderManager = new ShaderManager();
	g_ViewManager = new ViewManager(g_ShaderManager);
	g_Window = g_ViewManager->CreateDisplayWindow(WINDOW_TITLE);
	if (!g_Window) {
		Log("Failed to create GLFW window.");
	}
}

/***********************************************************
 * RenderLoop()
 ***********************************************************/
void RenderLoop() {
	while (!glfwWindowShouldClose(g_Window)) {
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		g_ViewManager->PrepareSceneView();
		g_SceneManager->RenderScene();
		CheckGLError("Render Scene");

		glfwSwapBuffers(g_Window);
		glfwPollEvents();
	}
}

/***********************************************************
 * Cleanup()
 ***********************************************************/
void Cleanup() {
	if (g_SceneManager) delete g_SceneManager;
	if (g_ViewManager) delete g_ViewManager;
	if (g_ShaderManager) delete g_ShaderManager;

	glfwDestroyWindow(g_Window);
	glfwTerminate();

	g_SceneManager = nullptr;
	g_ViewManager = nullptr;
	g_ShaderManager = nullptr;
	g_Window = nullptr;

	Log("Resources cleaned up");
}

/***********************************************************
 * Log(message)
 ***********************************************************/
void Log(const std::string& message) {
	if (logFile.is_open()) {
		std::time_t now = std::time(nullptr);
		logFile << std::ctime(&now) << ": " << message << std::endl;
	}
}

/***********************************************************
 * CheckGLError(context)
 ***********************************************************/
bool CheckGLError(const std::string& context) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::string errorMessage = "OpenGL Error in " + context + ": " + std::to_string(error);
		Log(errorMessage);
		std::cerr << errorMessage << std::endl;
		return false;
	}
	return true;
}
