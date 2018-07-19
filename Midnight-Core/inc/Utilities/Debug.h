#ifndef _DEBUG_H_
#define	_DEBUG_H_

class Debug
{
public:
	static void Log(const char* _text);
	static void LogWarning(const char* _text);
	static void LogError(const char* _text);

protected:
private:
};

#endif