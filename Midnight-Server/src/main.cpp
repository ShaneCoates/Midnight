
#include "Server.h"

void main()
{
	Server* server = new Server();
	server->Run();
	delete server;
}

