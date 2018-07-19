#include "Utilities\Debug.h"
#include <iostream>
#include <Windows.h>
void Debug::Log(const char* _text)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	std::cout << "Log: " << _text << std::endl;
}
void Debug::LogWarning(const char* _text)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
	std::cout << "Warning: " << _text << std::endl;
}
void Debug::LogError(const char* _text)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	std::cout << "Error: " << _text << std::endl;
}