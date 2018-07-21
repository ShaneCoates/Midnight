#include "Game/PlayerController.h"
#include "Utilities/Utilities.h"
#include "Game.h"
#include "Game/GameRules.h"
#include "glfw/glfw3.h"
#include "glm/ext.hpp"
#include "aieutilities/Gizmos.h"
#include "Utilities/Debug.h"
#include "Networking/NetworkManager_Client.h"
#include "MessageIdentifiers.h"
#define _USE_MATH_DEFINES
#include <math.h>

PlayerController::PlayerController(GLFWwindow* _window, bool _takesLocalInput)
{
	m_window = _window;
	m_takesLocalInput = _takesLocalInput;
	m_position = glm::vec3(0.0f, 0.5f, 0.0f);
	m_friction = GameRules::Player::k_friction;
	m_acceleration = GameRules::Player::k_acceleration;
	m_maxSpeed = GameRules::Player::k_maxSpeed;

}

PlayerController::~PlayerController()
{

}

void PlayerController::Update(double _dt)
{
	m_timeSinceSpawn += _dt;
	if(m_takesLocalInput)
	{
		UpdateInput(_dt);
	}

	m_position = Game::GetNetworkManager()->m_playerInfo.position;
	m_rotation = Game::GetNetworkManager()->m_playerInfo.rotation;

	UpdateGunPositions(_dt);
	UpdateFlashColor(_dt);
}

void PlayerController::Draw(Camera * _camera)
{
	glm::mat4 trans(1);
	trans = glm::rotate(trans, m_rotation, glm::vec3(0, 1, 0));

	glm::mat4 rightTrans = glm::translate(trans, glm::vec3(-0.24f, 0.0f, (0.1f * (1 - m_miscData.x)) + 0.25f));
	glm::mat4 leftTrans = glm::translate(trans, glm::vec3(0.24f, 0.0f, (0.1f * (1 - m_miscData.y)) + 0.25f));

	glm::mat4 rightFinalTrans = glm::translate(rightTrans, glm::vec3(0.0f, 0.0f, 5.0f));
	glm::mat4 leftFinalTrans = glm::translate(leftTrans, glm::vec3(0.0f, 0.0f, 5.0f));

	if(m_timers.x < 0.2f)
		Gizmos::addLine(rightTrans[3].xyz + m_position, rightFinalTrans[3].xyz + m_position, glm::vec4(0.5f, 0, 0, 1), glm::vec4(0.0f, 0, 0, 0));

	if(m_timers.y < 0.2f)
		Gizmos::addLine(leftTrans[3].xyz + m_position, leftFinalTrans[3].xyz + m_position, glm::vec4(0.5f, 0, 0, 1), glm::vec4(0.0f, 0, 0, 0));

}

void PlayerController::UpdateInput(double _dt)
{
	bool rightButtonDown = glfwGetMouseButton(m_window, 1);
	if(rightButtonDown)
	{
		if(m_rightButtonDown)
		{
			double x, y;
			glfwGetCursorPos(m_window, &x, &y);
			glm::vec2 mousePos = glm::vec2(x - (Game::k_windowWidth * 0.5f), y - (Game::k_windowHeight * 0.5f));
			//m_rotation -= glm::radians(mousePos.x * m_rotationSpeed * _dt);
			Game::GetNetworkManager()->SendInputStatus(ID_PLAYER_INPUT_ROTATION_DELTA, glm::radians(mousePos.x * m_rotationSpeed * _dt));
		}
		glfwSetCursorPos(m_window, Game::k_windowWidth * 0.5f, Game::k_windowHeight * 0.5f);
	}

	m_rightButtonDown = rightButtonDown;
	glfwSetInputMode(m_window, GLFW_CURSOR, m_rightButtonDown ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);


	bool tempButtonDown = (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS);
	bool tempCurrentState = (m_inputStates & (1 << GameRules::InputKeys::W));
	if(tempButtonDown != tempCurrentState)
	{
		Game::GetNetworkManager()->SendInputStatus(tempButtonDown ? ID_PLAYER_INPUT_W_DOWN : ID_PLAYER_INPUT_W_UP);
		m_inputStates ^= (1 << GameRules::InputKeys::W);
	}

	tempButtonDown = (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS);
	tempCurrentState = (m_inputStates & (1 << GameRules::InputKeys::S));
	if(tempButtonDown != tempCurrentState)
	{
		Game::GetNetworkManager()->SendInputStatus(tempButtonDown ? ID_PLAYER_INPUT_S_DOWN : ID_PLAYER_INPUT_S_UP);
		m_inputStates ^= (1 << GameRules::InputKeys::S);
	}

	 tempButtonDown = (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS);
	 tempCurrentState = (m_inputStates & (1 << GameRules::InputKeys::A));
	if(tempButtonDown != tempCurrentState)
	{
		Game::GetNetworkManager()->SendInputStatus(tempButtonDown ? ID_PLAYER_INPUT_A_DOWN : ID_PLAYER_INPUT_A_UP);
		m_inputStates ^= (1 << GameRules::InputKeys::A);
	}

	 tempButtonDown = (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS);
	 tempCurrentState = (m_inputStates & (1 << GameRules::InputKeys::D));
	if(tempButtonDown != tempCurrentState)
	{
		Game::GetNetworkManager()->SendInputStatus(tempButtonDown ? ID_PLAYER_INPUT_D_DOWN : ID_PLAYER_INPUT_D_UP);
		m_inputStates ^= (1 << GameRules::InputKeys::D);
	}
	

	if(glfwGetMouseButton(m_window, 0))
	{
		if(m_timers.x >= 1.0f && m_timers.y > 0.5f)
			m_timers.x = 0.0f;
		else if(m_timers.y > 1.0f && m_timers.x > 0.5f)
			m_timers.y = 0.0f;
	}
	if(glfwGetKey(m_window, GLFW_KEY_3) == GLFW_PRESS)
	{
		if(m_timers.z >= 1.0f)
			m_timers.z = 0.0f;
	}
}

void PlayerController::UpdateGunPositions(double _dt)
{
	if(m_timers.x < 1.0f)
	{
		m_timers.x += _dt * 5.0f;
		if(m_timers.x < 0.2f)
		{
			m_miscData.x = Utilities::Lerp(0.0f, 1.0f, m_timers.x * 5);
		}
		else
		{
			m_miscData.x = Utilities::Lerp(1.0f, 0.0f, (m_timers.x - 0.2f) * 5 / 4);
		}
	}
	else
	{
		m_miscData.x = 0.0f;
	}

	if(m_timers.y < 1.0f)
	{
		m_timers.y += _dt * 5.0f;
		if(m_timers.y < 0.2f)
		{
			m_miscData.y = Utilities::Lerp(0.0f, 1.0f, m_timers.y * 5);
		}
		else
		{
			m_miscData.y = Utilities::Lerp(1.0f, 0.0f, (m_timers.y - 0.2f) * 5 / 4);
		}
	}
	else
	{
		m_miscData.y = 0.0f;
	}

}

void PlayerController::UpdateFlashColor(double _dt)
{
	if(m_timers.z < 1.0f)
	{
		m_timers.z += _dt * 8.0f;
		m_miscData.z = (1 - abs((m_timers.z - 0.5f) * 2));
	}
	else
	{
		m_miscData.z = 0.0f;
	}
}

glm::vec4 PlayerController::GetPlayerPositionAndRotation()
{
	return glm::vec4(m_position, m_rotation);
}

glm::vec4 PlayerController::GetPlayerData()
{
	return m_miscData;
}