/*
	Author: Shane Coates
	Description: Main Server class
*/

#ifndef _SERVER_H_
#define	_SERVER_H_

#define DRAW_SERVER 1

struct GLFWwindow;
class ServerManager;
class GameStateManager;

class Server
{
public:

	//Constructor
	Server();
	//Destructor
	~Server();

	//Main Game loop
	void Run();

	const static int k_windowWidth = 1280;
	const static int k_windowHeight = 800;

protected:
private:
	//Function to return DeltaTime
	double GetDeltaTime();

	//Variables to calculate Delta Time
	double currentFrame;
	double deltaTime;
	double lastFrame;
	
	//Window
	GLFWwindow* m_gameWindow;
	
	ServerManager* m_serverManager;


};

#endif