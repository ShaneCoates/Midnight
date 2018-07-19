#include "Rendering/Material.h"
#include "Utilities/Utilities.h"
#include "gl/gl_core_4_4.h"

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
	m_materialID = handle;
	return handle;
}

void Material::Render()
{

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
