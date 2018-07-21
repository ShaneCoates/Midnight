/*
	Author: Shane Coates
	Description: Main Game class
*/

#ifndef _GAME_H_
#define	_GAME_H_
#include "gl/gl_core_4_4.h"
struct GLFWwindow;
class NetworkManager_Client;
class GameStateManager;

class Game
{
public:

	//Constructor
	Game();
	//Destructor
	~Game();

	//Main Game loop
	void Run();

private:

	//Function to return DeltaTime
	double GetDeltaTime();

public:

	const static int k_windowWidth = 1280;
	const static int k_windowHeight = 800;

	inline static NetworkManager_Client* GetNetworkManager() { return m_networkManager;}

private:

	//Variables to calculate Delta Time
	double currentFrame;
	double deltaTime;
	double lastFrame;
	
	//Window
	GLFWwindow* m_gameWindow;
	
	//Game State Manager
	GameStateManager* m_gameStateManager;

	//Network Manager
	static NetworkManager_Client* m_networkManager;

};

#endif