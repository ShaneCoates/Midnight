#ifndef	_PLAYER_CONTROLLER_H_
#define _PLAYER_CONTROLLER_H_

#include "glm/glm.hpp"
struct GLFWwindow;
class Camera;
class PlayerController
{
public:

	PlayerController(GLFWwindow* _window, bool _takesLocalInput = false);
	~PlayerController();

	void Update(double _dt);

	void Draw(Camera* _camera);

	glm::vec4 GetPlayerPositionAndRotation();
	glm::vec4 GetPlayerData();


private:

	void UpdateInput(double _dt);
	void UpdateGunPositions(double _dt);
	void UpdateFlashColor(double _dt);
	
	GLFWwindow* m_window;

	bool m_takesLocalInput;

	glm::vec4 m_timers;
	glm::vec3 m_velocity;
	glm::vec3 m_position;
	float m_rotation;
	glm::vec4 m_miscData;
	double m_timeSinceSpawn = 0.0f;

	//cache some game rules
	float m_friction;
	float m_acceleration;
	float m_maxSpeed;

	float m_rotationSpeed = 3.0f;
	bool m_rightButtonDown = false;

};

#endif
