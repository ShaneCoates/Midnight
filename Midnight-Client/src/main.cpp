// Networking-FirstTest-Client.cpp : Defines the entry point for the console application.
//

#include "Game.h"

void main()
{
	Game* theGame = new Game();
	theGame->Run();
	delete theGame;
}

