#include "Game.h"

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

#include "GameState/GameStateManager.h"
#include "GameState/MainState.h"
#include "GameState/LobbyState.h"

#include "Utilities/Debug.h"

Game::Game() 
{
	srand(time(NULL));
	currentFrame = 0;
	deltaTime = 0;
	lastFrame = 0;
	
	if (glfwInit() == false) {
		printf("Failed to initialise GLFW");
		return;
	}

	m_gameWindow = glfwCreateWindow(k_windowWidth, k_windowHeight, "Midnight - Client - Shane Coates", nullptr, nullptr);
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

	m_gameStateManager = new GameStateManager();
	m_gameStateManager->RegisterState("Lobby", new LobbyState(m_gameWindow, m_gameStateManager));
	m_gameStateManager->RegisterState("Main", new MainState(m_gameWindow, m_gameStateManager));
	m_gameStateManager->Push("Main");


}
Game::~Game() 
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_gameWindow);
	glfwTerminate();

	delete m_gameStateManager;
}

void Game::Run() {
	glEnable(GL_DEBUG_OUTPUT);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (glfwWindowShouldClose(m_gameWindow) == false) {
		double dt = GetDeltaTime();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		m_gameStateManager->Update(dt);
		m_gameStateManager->Draw();
		
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(m_gameWindow);
	}
}

double Game::GetDeltaTime(){
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	return deltaTime;
}