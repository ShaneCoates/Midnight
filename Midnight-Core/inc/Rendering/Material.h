#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <map>
#include "glm/glm.hpp"
#include "gl/gl_core_4_4.h"
enum MaterialShaders
{
	Vertex					= 0x1,
	Fragment				= 0x2,
	Geometry				= 0x3,
	TessellationControl		= 0x4,
	TessellationEvaluation	= 0x5,
	MAX
};

class Material
{
public:
	Material();
	~Material();

	void LoadFromFile(MaterialShaders _shaderType, const char* _filePath);
	unsigned int CreateProgram();
	void StartRender();

	void SetVertices(const char* _property, GLsizeiptr _size, void * _vertices);

	//Material uniform
	void SetMaterialUniformAttrib(const char* _attrib, int _value);
	void SetMaterialUniformAttrib(const char* _attrib, float _value);
	void SetMaterialUniformAttrib(const char* _attrib, glm::vec2 _value);
	void SetMaterialUniformAttrib(const char* _attrib, glm::vec3 _value);
	void SetMaterialUniformAttrib(const char* _attrib, glm::vec4 _value);

	void AddAttribLocation(const char * _attribName);

	void AddUniformLocation(const char* _attribName);
private:

	unsigned int GetGLShaderType(MaterialShaders _shaderType);

	unsigned int m_shadersUsed = 0;
	unsigned int m_shaderIDs[MaterialShaders::MAX];


	unsigned int m_materialID;
	unsigned int m_vao;
	unsigned int m_vbo;

	
	std::map<const char*, unsigned int> m_attribLocations;
	std::map<const char*, unsigned int> m_uniformLocations;
};

#endif 
