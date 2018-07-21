#include "GameState/LobbyState.h"
#include "Game.h"
#include "GameState/GameStateManager.h"
#include "glfw/glfw3.h"
#include "imgui\imgui.h"
#include "Networking\NetworkManager_Client.h"

void LobbyState::Init(GLFWwindow* _window, GameStateManager* _gameStateManager)
{
	m_window = _window;
	m_gameStateManager = _gameStateManager;
	
	m_lobbyScreenPos.x = Game::k_windowWidth * 0.5f - (k_lobbyScreenSize.x * 0.5f);
	m_lobbyScreenPos.y = Game::k_windowHeight * 0.5f - (k_lobbyScreenSize.y * 0.5f);

	m_ImGuiWindowFlag |= (int)ImGuiWindowFlags_NoMove;
	m_ImGuiWindowFlag |= (int)ImGuiWindowFlags_NoCollapse;
	m_ImGuiWindowFlag |= (int)ImGuiWindowFlags_NoResize;
	m_ImGuiWindowFlag |= (int)ImGuiWindowFlags_NoTitleBar;

	m_networkManager = Game::GetNetworkManager();
}

LobbyState::~LobbyState()
{

}

void LobbyState::Update(double _dt)
{
	
}

void LobbyState::Draw()
{
	DrawLobbyPanel();
}

void LobbyState::DrawLobbyPanel()
{
	ImGui::Begin("Lobby", &m_ImGuiWindowOpen, m_ImGuiWindowFlag);
	ImGui::SetWindowPos(m_lobbyScreenPos);
	ImGui::SetWindowSize(k_lobbyScreenSize);

	ImGui::Spacing();
	ImGui::Text("LOBBY");
	ImGui::Spacing();
	ImGui::Separator();

	ImGui::InputText("Name", m_playerName, IM_ARRAYSIZE(m_playerName));

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::InputText("Address", m_serverAddress, IM_ARRAYSIZE(m_serverAddress));

	ImGui::InputInt("Port", &m_serverPort);

	//ImGui::Spacing();
	if(ImGui::Button("Connect"))
	{
		m_networkManager->AttemptConnect(m_serverAddress, m_serverPort, [=]
		{
			m_networkManager->SetPlayerName(m_playerName);
			m_gameStateManager->Push("Main");
			m_gameStateManager->Pop();
		});
	}



	ImGui::End();
}