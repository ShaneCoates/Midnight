#ifndef _UTILITIES_H_
#define	_UTILITIES_H_

#include <vector>
#include <string>
class Utilities
{
public:
	static std::vector<std::string> SplitString(const std::string &s, char delim);
	static std::vector<std::string> SplitString(const std::string &s, const char* delims);
	static unsigned char* FileToBuffer(const char* _filePath);
	static float Lerp(float _a, float _b, float _t);
};

#endif