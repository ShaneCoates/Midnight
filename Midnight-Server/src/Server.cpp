#include "Server.h"

#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>

#include "gl\gl_core_4_4.h"
#include "glfw\glfw3.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw.h"
#include "imgui\imgui_impl_opengl3.h"

#include "ServerManager.h"


Server::Server()
{
	srand(time(NULL));
	currentFrame = 0;
	deltaTime = 0;
	lastFrame = 0;
	m_serverManager = new ServerManager();
	m_serverManager->Init(12345);

#if DRAW_SERVER
	if (glfwInit() == false) {
		printf("Failed to initialize GLFW");
		return;
	}

	m_gameWindow = glfwCreateWindow(k_windowWidth, k_windowHeight, "Midnight - Server - Shane Coates", nullptr, nullptr);
	if (m_gameWindow == nullptr) {
		printf("Failed to create Game Window");
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(m_gameWindow);
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		printf("Failed to load OpenGL Functions");
		glfwDestroyWindow(m_gameWindow);
		glfwTerminate();
		return;
	}
	glfwSwapInterval(1); // Enable vsync
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplGlfw_InitForOpenGL(m_gameWindow, true);
	ImGui_ImplOpenGL3_Init();

	ImGui::StyleColorsDark();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
#endif
}

Server::~Server() 
{
#if DRAW_SERVER
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_gameWindow);
	glfwTerminate();
#endif
	delete m_serverManager;
}

void Server::Run() 
{
#if DRAW_SERVER
	while (glfwWindowShouldClose(m_gameWindow) == false) 
	{
		double dt = GetDeltaTime();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		m_serverManager->Update();
		m_serverManager->Draw();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(m_gameWindow);
	}
#else
	while(true)
	{
		m_serverManager->Update();
	}
#endif
}

double Server::GetDeltaTime(){
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	return deltaTime;
}