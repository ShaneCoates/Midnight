#ifndef _MATERIAL_H_
#define _MATERIAL_H_

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
	void Render();

private:

	unsigned int GetGLShaderType(MaterialShaders _shaderType);

	unsigned int m_shadersUsed = 0;

	unsigned int m_materialID;
	unsigned int m_shaderIDs[MaterialShaders::MAX];
	
};

#endif 
