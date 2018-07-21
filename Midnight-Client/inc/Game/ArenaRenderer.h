#ifndef _ARENA_RENDERER_H_
#define _ARENA_RENDERER_H_

#include "glm/glm.hpp"
#include "gl/gl_core_4_4.h"

class Camera;
class PlayerController;
struct GLFWwindow;
class ArenaRenderer
{
public:
	ArenaRenderer(GLFWwindow* _window, unsigned int _windowWidth, unsigned int _windowHeight);
	~ArenaRenderer();

	void Update(double _dt);
	void Draw(Camera* _camera);
	void DrawGUI();

	void UpdatePlayer(unsigned int _playerIndex, glm::vec4 _playerPosAndRot, glm::vec4 _otherData);

private:

	void UpdateMaterial();

	double m_time;

	glm::vec4 m_playerPos[1];
	glm::vec4 m_playerData[1];

	class Material* m_material;
	bool m_materialShouldUpdate = true; //default to true so we make sure we hit everything first frame
	
	unsigned int m_programID;


	GLfloat m_vertices[16] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	bool m_MSAA = true;
	bool m_AO = true;

	float m_focalLength = 1.85f;								// Distance between eye and image-plane
	float m_zNear = 0.0f;									// Near plane distance from camera
	float m_zFar = 100.0f;								// Far plane distance from camera

	// Raymarch parameters
	int m_rmSteps = 64;
	float m_rmEpsilon = 0.001f;

	glm::vec4 m_skyColor = glm::vec4(47.0f / 255.0f, 230.0f / 255.0f, 222.0f / 255.0f, 1.0f);
	glm::vec4 m_ambient = glm::vec4(28.0f / 255.0f, 48.0f / 255.0f, 65.0f / 255.0f, 1.0f);
	glm::vec3 m_directionLightDir = glm::normalize(glm::vec3(1.0f, -1.0f, -0.7f));
	glm::vec4 m_directionalLightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 m_pointLightPosition = glm::vec3(0.0f, 2.0f, 0.0f);
	glm::vec4 m_pointLightColor = glm::vec4(0.67f, 0.87f, 0.93f, 1.0f);
};

#endif
