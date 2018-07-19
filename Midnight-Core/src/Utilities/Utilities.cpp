#include "Utilities\Utilities.h"
#include <sstream>
std::vector<std::string> Utilities::SplitString(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) 
	{
		if (item != "")
		{
			elems.push_back(item);
		}
	}
	return elems;
}
std::vector<std::string> Utilities::SplitString(const std::string &s, const char* delims)
{
	std::vector<std::string> elems;
	std::stringstream stringStream(s);
	std::string line;
	while (std::getline(stringStream, line))
	{
		std::size_t prev = 0, pos;
		while ((pos = line.find_first_of(delims, prev)) != std::string::npos)
		{
			if (pos > prev)
				elems.push_back(line.substr(prev, pos - prev));
			prev = pos + 1;
		}
		if (prev < line.length())
			elems.push_back(line.substr(prev, std::string::npos));
	}
	return elems;
}

unsigned char* Utilities::FileToBuffer(const char* _filePath)
{
	FILE* pFile = fopen(_filePath, "rb");
	if(pFile == nullptr)
	{
		printf("Error: Unable to open file '%s' for reading!\n", _filePath);
		return nullptr;
	}
	fseek(pFile, 0, SEEK_END);
	unsigned int uiLength = (unsigned int)ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	unsigned char* acBuffer = new unsigned char[uiLength + 1];
	memset(acBuffer, 0, uiLength + 1);
	fread(acBuffer, sizeof(unsigned char), uiLength, pFile);
	fclose(pFile);
	return acBuffer;
}

float Utilities::Lerp(float _a, float _b, float _t)
{
	return _a + _t * (_b - _a);
}
