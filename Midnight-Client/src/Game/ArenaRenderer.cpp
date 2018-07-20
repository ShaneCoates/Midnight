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
#include "Game/ArenaRendererProperties.h"

ArenaRenderer::ArenaRenderer(GLFWwindow* _window, unsigned int _windowWidth, unsigned int _windowHeight)
{
	m_material = new Material();
	m_material->LoadFromFile(MaterialShaders::Vertex, "res/shaders/MidnightGameRaymarch.vs");
	m_material->LoadFromFile(MaterialShaders::Fragment, "res/shaders/MidnightGameRaymarch.fs");
	m_programID = m_material->CreateProgram();

	glUseProgram(m_programID);

	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_resolution, glm::vec2(_windowWidth, _windowHeight));
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_aspectRatio, _windowWidth / (float)_windowHeight);

	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_zNear, m_zNear);
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_zFar, m_zFar);

	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_rmEpsilon, m_rmEpsilon);
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_rmSteps, m_rmSteps);

	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_skyColor, m_skyColor);
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_ambient, m_ambient);

	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_focalLength, m_focalLength);
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

	m_material->StartRender();
	m_material->SetVertices(ArenaRendererProperties::k_position, sizeof(m_vertices), m_vertices);

	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_time,					(float)m_time);
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_camUp,					glm::vec3(mat[1]));
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_camRight,				glm::vec3(mat[0]));
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_camForward,				glm::vec3(mat[2] * -1));
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_eye,					_camera->GetPosition());
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_directionalLightDir,	m_directionLightDir);
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_directionalLightColor,	m_directionalLightColor);
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_pointLightPos,			m_pointLightPosition);
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_pointLightColor,		m_pointLightColor);
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_focalLength,			m_focalLength);
	
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_player1Pos, m_playerPos[0]);
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_player1Data, m_playerData[0]);


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
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_MSAA, m_MSAA);
	m_material->SetMaterialUniformAttrib(ArenaRendererProperties::k_AO, m_AO);
}

