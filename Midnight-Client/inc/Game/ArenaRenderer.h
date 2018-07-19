#ifndef _ARENA_RENDERER_H_
#define _ARENA_RENDERER_H_

#include <map>
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

	void AddAttribLocation(const char* _attribName);
	void AddUniformLocation(const char* _attribName);

	void UpdateMaterial();


	double m_time;

	glm::vec4 m_playerPos[1];
	glm::vec4 m_playerData[1];

	class Material* m_material;
	bool m_materialShouldUpdate = false;

	//TODO:
	//Not happy with this solution - unoptimised and leaves a lot of chance for human error
	//Find a better, more robust solution. At least try const strings for param names
	std::map<const char*, unsigned int> m_materialAttribLocations;
	std::map<const char*, unsigned int> m_materialUniformLocations;

	
	unsigned int m_programID;
	unsigned int m_vao;
	unsigned int m_vbo;

	GLfloat m_vertices[16] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	bool m_MSAA = false;
	bool m_AO = true;

	float m_focalLength = 1.85f;								// Distance between eye and image-plane
	float m_zNear = 0.0f;									// Near plane distance from camera
	float m_zFar = 100.0f;								// Far plane distance from camera

	// Raymarch parameters
	int m_rmSteps = 64;
	float m_rmEpsilon = 0.001f;

	glm::vec4 m_skyColor = glm::vec4(47.0f / 255.0f, 230.0f / 255.0f, 222.0f / 255.0f, 1.0f);
	glm::vec4 m_ambient = glm::vec4(28.0f / 255.0f, 48.0f / 255.0f, 65.0f / 255.0f, 1.0f);
	glm::vec3 m_light0Position = glm::normalize(glm::vec3(1.0f, -1.0f, -0.7f));
	glm::vec4 m_light0Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 m_light1Position = glm::vec3(0.0f, 2.0f, 0.0f);
	glm::vec4 m_light1Color = glm::vec4(0.67f, 0.87f, 0.93f, 1.0f);
};

#endif