#include "GameState/MainState.h"
#include "Game.h"
#include "Camera/FollowCamera.h"
#include "glfw/glfw3.h"
#include "imgui\imgui.h"
#include "Rendering\Material.h"
#include "gl/gl_core_4_4.h"
#include "Game/ArenaRenderer.h"
#include "Game/PlayerController.h"
#include "aieutilities/Gizmos.h"

#define _USE_MATH_DEFINES
#include <math.h>

void MainState::Init(GLFWwindow* _window, GameStateManager* _gameStateManager) 
{
	m_window = _window;
	m_gameStateManager = _gameStateManager;
	m_followCamera = new FollowCamera();
	m_followCamera ->SetPerspective(1.0f, Game::k_windowWidth / (float)Game::k_windowHeight, 0.1f, 100.0f);
	m_followCamera->SetOffsetFromTarget(glm::vec3(0.0f, 1.5f, -2.0f));
	m_activeCamera = m_followCamera;

	m_player = new PlayerController(_window, true);

	m_ImGuiWindowFlag |= (int)ImGuiWindowFlags_NoMove;
	m_ImGuiWindowFlag |= (int)ImGuiWindowFlags_NoCollapse;
	m_ImGuiWindowFlag |= (int)ImGuiWindowFlags_NoResize;
	m_ImGuiWindowFlag |= (int)ImGuiWindowFlags_NoTitleBar;

	m_arenaRenderer = new ArenaRenderer(m_window, Game::k_windowWidth, Game::k_windowHeight);
	Gizmos::create();
}
MainState::~MainState()
{
	delete m_player;
	delete m_arenaRenderer;
	delete m_followCamera;
}

void MainState::Update(double _dt) 
{
	m_time += _dt;

	m_arenaRenderer->Update(_dt);
	m_activeCamera->Update(_dt);

	m_player->Update(_dt);
	m_arenaRenderer->UpdatePlayer(0, m_player->GetPlayerPositionAndRotation(), m_player->GetPlayerData());
	glm::vec3 playerPos = m_player->GetPlayerPositionAndRotation().xyz;
	playerPos.y = 0.5f;
	m_followCamera->UpdatePosition(playerPos, glm::vec3(0, 0.2f, 0));
	
	
	DrawGUI();
}
void MainState::Draw() 
{
	Gizmos::clear();

	DrawArena();	
	m_player->Draw(m_activeCamera);

	Gizmos::draw(m_activeCamera->GetProjectionView());
}

const void MainState::DrawArena()
{
	m_arenaRenderer->Draw(m_activeCamera);
}

void MainState::DrawGUI() 
{
	ImGui::Begin("Debug", &m_ImGuiWindowOpen, m_ImGuiWindowFlag);
	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(300, Game::k_windowHeight));
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);

	ImGui::Separator();

	m_arenaRenderer->DrawGUI();


	ImGui::End();
}
