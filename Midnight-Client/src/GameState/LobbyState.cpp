#include "GameState/LobbyState.h"
#include "Game.h"
#include "GameState/GameStateManager.h"
#include "glfw/glfw3.h"
#include "imgui\imgui.h"
#include "raknet\MessageIdentifiers.h"
#include "raknet\RakNetTypes.h"

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

	m_peerInterface = RakNet::RakPeerInterface::GetInstance();
	m_peerInterface->Startup(1, &RakNet::SocketDescriptor(), 1);

	
}

LobbyState::~LobbyState()
{
	RakNet::RakPeerInterface::DestroyInstance(m_peerInterface);

}

void LobbyState::Update(double _dt)
{
	for (m_packet = m_peerInterface->Receive(); m_packet; m_peerInterface->DeallocatePacket(m_packet), m_packet = m_peerInterface->Receive())
	{
		switch (m_packet->data[0])
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
			m_hasConnected = true;

			m_bitStream.Reset();
			m_bitStream.Write((RakNet::MessageID)ID_SET_PLAYER_NAME);
			m_bitStream.Write(m_playerName);
			m_peerInterface->Send(&m_bitStream, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE, 0, m_packet->systemAddress, false);
			break;
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			printf("Another client has disconnected.\n");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			printf("Another client has lost the connection.\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			printf("Another client has connected.\n");
			break;
		case ID_NEW_INCOMING_CONNECTION:
			printf("A connection is incoming.\n");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("The server is full.\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			printf("We have been disconnected.\n");
			break;
		case ID_CONNECTION_LOST:
			printf("Connection lost.\n");
			break;
		case ID_SET_PLAYER_ID:
			unsigned int id = m_packet->data[1];
			break;
		}
	}
	
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

	if(m_hasConnected)
	{
		m_gameStateManager->Push("Main");
		m_gameStateManager->Pop();
	}
	else
	{
		ImGui::InputText("Name", m_playerName, IM_ARRAYSIZE(m_playerName));

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::InputText("Address", m_serverAddress, IM_ARRAYSIZE(m_serverAddress));

		ImGui::InputInt("Port", &m_serverPort);

		//ImGui::Spacing();
		if (ImGui::Button("Connect"))
		{
			m_peerInterface->Connect(m_serverAddress, m_serverPort, 0, 0);
		}

	}
	
	ImGui::End();
}