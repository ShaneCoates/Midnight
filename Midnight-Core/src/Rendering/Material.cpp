#include "Rendering/Material.h"
#include "Utilities/Utilities.h"
#include "gl/gl_core_4_4.h"
#include "glm/ext.hpp"

Material::Material()
{
}

Material::~Material()
{
}

void Material::LoadFromFile(MaterialShaders _shaderType, const char * _filePath)
{
	//turn on this shader type
	m_shadersUsed |= _shaderType;

	int success = GL_FALSE;
	unsigned int handle = glCreateShader(GetGLShaderType(_shaderType));
	unsigned char* source = Utilities::FileToBuffer(_filePath);
	glShaderSource(handle, 1, (const char**)&source, 0);
	glCompileShader(handle);
	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetShaderInfoLog(handle, infoLogLength, 0, infoLog);
		printf("Error: Failed to compile shader!\n");
		printf("%s", infoLog);
		printf("\n");
		delete[] infoLog;
		return;
	}
	delete[] source;
	m_shaderIDs[_shaderType] = handle;
}

unsigned int Material::CreateProgram()
{
	int success = GL_FALSE;
	unsigned int handle = glCreateProgram();

	for(unsigned int i = 0x1; i < MaterialShaders::MAX; i++)
	{
		if(m_shadersUsed & i)
		{
			glAttachShader(handle, m_shaderIDs[i]);
		}
	}
	glLinkProgram(handle);
	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	if(success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(handle, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s", infoLog);
		printf("\n");
		delete[] infoLog;
		return 0;
	}
	//Create buffer objects
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	m_materialID = handle;
	return handle;
}

void Material::StartRender()
{
	glUseProgram(m_materialID);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

}

void Material::SetVertices(const char* _property, GLsizeiptr _size, void* _vertices)
{
	glBufferData(GL_ARRAY_BUFFER, _size, _vertices, GL_STATIC_DRAW);

	if(m_attribLocations.count(_property) == 0)
	{
		AddAttribLocation(_property);
	}

	glEnableVertexAttribArray(m_attribLocations[_property]);
	glVertexAttribPointer(m_attribLocations[_property], 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
}

void Material::SetMaterialUniformAttrib(const char* _attrib, int _value)
{
	if(m_uniformLocations.count(_attrib) == 0)
	{
		AddUniformLocation(_attrib);
	}
	glUniform1i(m_uniformLocations[_attrib], _value);
}

void Material::SetMaterialUniformAttrib(const char* _attrib, float _value)
{
	if(m_uniformLocations.count(_attrib) == 0)
	{
		AddUniformLocation(_attrib);
	}
	glUniform1f(m_uniformLocations[_attrib], _value);
}

void Material::SetMaterialUniformAttrib(const char* _attrib, glm::vec2 _value)
{
	if(m_uniformLocations.count(_attrib) == 0)
	{
		AddUniformLocation(_attrib);
	}
	glUniform2fv(m_uniformLocations[_attrib], 1, value_ptr(_value));
	
}

void Material::SetMaterialUniformAttrib(const char* _attrib, glm::vec3 _value)
{
	if(m_uniformLocations.count(_attrib) == 0)
	{
		AddUniformLocation(_attrib);
	}
	glUniform3fv(m_uniformLocations[_attrib], 1, value_ptr(_value));

}

void Material::SetMaterialUniformAttrib(const char* _attrib, glm::vec4 _value)
{
	if(m_uniformLocations.count(_attrib) == 0)
	{
		AddUniformLocation(_attrib);
	}
	glUniform4fv(m_uniformLocations[_attrib], 1, value_ptr(_value));
	
}

void Material::AddAttribLocation(const char* _attribName)
{
	unsigned int attribLoc = glGetAttribLocation(m_materialID, _attribName);
	if(attribLoc != -1)
	{
		m_attribLocations[_attribName] = attribLoc;
	}
}

void Material::AddUniformLocation(const char* _attribName)
{
	unsigned int attribLoc = glGetUniformLocation(m_materialID, _attribName);
	if(attribLoc != -1)
	{
		m_uniformLocations[_attribName] = attribLoc;
	}
}



unsigned int Material::GetGLShaderType(MaterialShaders _shaderType)
{
	switch(_shaderType)
	{
	case Vertex:
		return GL_VERTEX_SHADER;
		break;
	case Fragment:
		return GL_FRAGMENT_SHADER;
		break;
	case Geometry:
		return GL_GEOMETRY_SHADER;
		break;
	case TessellationControl:
		return GL_TESS_CONTROL_SHADER;
		break;
	case TessellationEvaluation:
		return GL_TESS_EVALUATION_SHADER;
		break;
	default:
		return GL_VERTEX_SHADER;
		break;
	}
}
