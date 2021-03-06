/*
Author: Shane Coates
Description: Test state for shadows
*/

#ifndef _LOBBY_STATE_H_
#define	_LOBBY_STATE_H_

#include "GameState.h"
#include "imgui\imgui.h"
#include "RakPeerInterface.h"
#include "raknet\BitStream.h"

struct GLFWwindow;
class GameStateManager;
class LobbyState : public IGameState
{
public:
	//Constructor inherits from IGameState and calls the Init function
	LobbyState(GLFWwindow* _window, GameStateManager* _gameStateManager) : IGameState()
	{
		Init(_window, _gameStateManager);
	}
	//Destructor
	~LobbyState();

	//Initialise the gamestate
	void Init(GLFWwindow* _window, GameStateManager* _gameStateManager);

	//Update/Draw functions
	void Update(double _dt);
	void Draw();

private:

	void DrawLobbyPanel();

	GLFWwindow * m_window;
	GameStateManager* m_gameStateManager;

	RakNet::RakPeerInterface *m_peerInterface;
	RakNet::Packet *m_packet;
	RakNet::BitStream m_bitStream;


	int m_ImGuiWindowFlag = 0;
	bool m_ImGuiWindowOpen = true;

	bool m_hasConnected = false;

	ImVec2 m_lobbyScreenPos;
	const ImVec2 k_lobbyScreenSize = ImVec2(250, 500);

	int m_serverPort = 12345;

	char m_playerName[16];
	char m_serverAddress[16] = "127.0.0.1";

};

#endif