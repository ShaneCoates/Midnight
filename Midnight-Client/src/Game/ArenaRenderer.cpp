#include "Game/ArenaRenderer.h"
#include "Rendering/Material.h"
#include "gl/gl_core_4_4.h"
#include "glm/ext.hpp"
#include <stdarg.h>
#include "Camera/Camera.h"
#include "Utilities/Debug.h"
#include "imgui/imgui.h"
#include "aieutilities/Gizmos.h"
#include "Game/GameRules.h"

ArenaRenderer::ArenaRenderer(GLFWwindow* _window, unsigned int _windowWidth, unsigned int _windowHeight)
{
	m_material = new Material();
	m_material->LoadFromFile(MaterialShaders::Vertex, "res/shaders/MidnightGameRaymarch.vs");
	m_material->LoadFromFile(MaterialShaders::Fragment, "res/shaders/MidnightGameRaymarch.fs");
	m_programID = m_material->CreateProgram();


	//Create buffer objects
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	AddAttribLocation("position");
	AddUniformLocation("m_resolution");
	AddUniformLocation("m_camUp");
	AddUniformLocation("m_camRight");
	AddUniformLocation("m_camForward");
	AddUniformLocation("m_eye");
	AddUniformLocation("m_focalLength");
	AddUniformLocation("m_zNear");
	AddUniformLocation("m_zFar");
	AddUniformLocation("m_aspectRatio");
	AddUniformLocation("m_rmSteps");
	AddUniformLocation("m_rmEpsilon");
	AddUniformLocation("m_skyColor");
	AddUniformLocation("m_ambient");
	AddUniformLocation("m_directionalLightDir");
	AddUniformLocation("m_directionalLightColor");
	AddUniformLocation("m_pointLightPos");
	AddUniformLocation("m_pointLightColor");
	AddUniformLocation("m_MSAA");
	AddUniformLocation("m_AO");
	AddUniformLocation("m_time");
	AddUniformLocation("m_player1Pos");
	AddUniformLocation("m_player1Data");


	glUseProgram(m_programID);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(m_materialAttribLocations["position"]);
	glVertexAttribPointer(m_materialAttribLocations["position"], 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glUniform2f(m_materialUniformLocations["m_resolution"], _windowWidth, _windowHeight);
	glUniform1f(m_materialUniformLocations["m_aspectRatio"], _windowWidth / (float)_windowHeight);

	glUniform1f(m_materialUniformLocations["m_zNear"], m_zNear);
	glUniform1f(m_materialUniformLocations["m_zFar"], m_zFar);

	glUniform1f(m_materialUniformLocations["m_rmEpsilon"], m_rmEpsilon);
	glUniform1i(m_materialUniformLocations["m_rmSteps"], m_rmSteps);

	glUniform4fv(m_materialUniformLocations["m_skyColor"], 1, value_ptr(m_skyColor));
	glUniform4fv(m_materialUniformLocations["m_ambient"], 1, value_ptr(m_ambient));


	glUniform1f(m_materialUniformLocations["m_focalLength"], m_focalLength);
}

ArenaRenderer::~ArenaRenderer()
{
	delete m_material;
}

void ArenaRenderer::Update(double _dt)
{
	m_time += _dt;
}

void ArenaRenderer::Draw(Camera* _camera)
{
	glm::mat4 mat = _camera->GetTransform();
	glUseProgram(m_programID);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(m_materialAttribLocations["position"]);
	glVertexAttribPointer(m_materialAttribLocations["position"], 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glUniform1f(m_materialUniformLocations["m_time"], m_time);
	glUniform3fv(m_materialUniformLocations["m_camUp"],				1, value_ptr(glm::vec3(mat[1])));
	glUniform3fv(m_materialUniformLocations["m_camRight"],			1, value_ptr(glm::vec3(mat[0])));
	glUniform3fv(m_materialUniformLocations["m_camForward"],		1, value_ptr(glm::vec3(mat[2] * -1)));
	glUniform3fv(m_materialUniformLocations["m_eye"],				1, value_ptr(_camera->GetPosition()));
	glUniform3fv(m_materialUniformLocations["m_directionalLightDir"],	1, value_ptr(m_light0Position));
	glUniform4fv(m_materialUniformLocations["m_directionalLightColor"],		1, value_ptr(m_light0Color));
	glUniform3fv(m_materialUniformLocations["m_pointLightPos"],	1, value_ptr(m_light1Position));
	glUniform4fv(m_materialUniformLocations["m_pointLightColor"],		1, value_ptr(m_light1Color));
	glUniform1f(m_materialUniformLocations["m_focalLength"], m_focalLength);

	
	glUniform4fv(m_materialUniformLocations["m_player1Pos"], 1, value_ptr(m_playerPos[0]));
	glUniform4fv(m_materialUniformLocations["m_player1Data"], 1, value_ptr(m_playerData[0]));

	//glUniform3fv(m_materialUniformLocations["m_eye"], 1, value_ptr(pos.xyz + glm::vec3(0.0f, 6.0f, 3.0f)));


	if(m_materialShouldUpdate)
	{
		UpdateMaterial();
		m_materialShouldUpdate = false;
	}

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	
}

void ArenaRenderer::DrawGUI()
{
	if(ImGui::Checkbox("MSAA", &m_MSAA))
	{
		m_materialShouldUpdate = true;
	}
	ImGui::SameLine();
	if(ImGui::Checkbox("AO", &m_AO))
	{
		m_materialShouldUpdate = true;
	}
	ImGui::SliderFloat("Focal Length", &m_focalLength, 1.0f, 5.0f);
}

void ArenaRenderer::UpdatePlayer(unsigned int _playerIndex, glm::vec4 _playerPosAndRot, glm::vec4 _otherData)
{
	m_playerPos[_playerIndex] = _playerPosAndRot;
	m_playerData[_playerIndex] = _otherData;
}

void ArenaRenderer::UpdateMaterial()
{
	glUniform1i(m_materialUniformLocations["m_MSAA"], m_MSAA);
	glUniform1i(m_materialUniformLocations["m_AO"], m_AO);
}

void ArenaRenderer::AddAttribLocation(const char* _attribName)
{
	unsigned int attribLoc = glGetAttribLocation(m_programID, _attribName);
	if(attribLoc != -1)
	{
		m_materialAttribLocations[_attribName] = attribLoc;
	}
	else
	{
		char buff[100];
		snprintf(buff, sizeof(buff), "%s not found", _attribName);
		Debug::LogWarning(buff);
	}
}

void ArenaRenderer::AddUniformLocation(const char* _attribName)
{
	unsigned int attribLoc = glGetUniformLocation(m_programID, _attribName);
	if(attribLoc != -1)
	{
		m_materialUniformLocations[_attribName] = attribLoc;
	}
	else
	{
		char buff[100];
		snprintf(buff, sizeof(buff), "%s not found", _attribName);
		Debug::LogWarning(buff);
	}
}

