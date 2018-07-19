/* 
	Author: Shane Coates
	Description: Test state for shadows
*/

#ifndef _MAIN_STATE_H_
#define	_MAIN_STATE_H_

#include "GameState.h"
#include "glm\glm.hpp"
#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR)/sizeof(*_ARR)))

struct GLFWwindow;
class GameStateManager;
class FollowCamera;
class Camera;
class ArenaRenderer;
class PlayerController;
class MainState : public IGameState
{
public:
	//Constructor inherits from IGameState and calls the Init function
	MainState(GLFWwindow* _window, GameStateManager* _gameStateManager) : IGameState()
	{
		Init(_window, _gameStateManager);
	}
	//Destructor
	~MainState();

	//Initialise the gamestate
	void Init(GLFWwindow* _window, GameStateManager* _gameStateManager);

	//Update/Draw functions
	void Update(double _dt);
	void Draw();

private:
	
	const void DrawArena();
	void DrawGUI();

	//Class references
	GLFWwindow * m_window;
	GameStateManager* m_gameStateManager;
	FollowCamera* m_followCamera;
	Camera* m_activeCamera;
	PlayerController* m_player;


	ArenaRenderer* m_arenaRenderer;

	int m_ImGuiWindowFlag = 0;
	bool m_ImGuiWindowOpen = true;

	float m_time = 0;

	glm::vec3 m_playerPos = glm::vec3(2, 0.5f, 2);
	
};

#endif