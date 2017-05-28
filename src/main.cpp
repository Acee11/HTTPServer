#include <iostream>
#include <cstdio>
#include <fstream>  
#include "HttpMsg.h"
#include "ClientHandler.h"
#include "Server.h"


int main(int argc, char** argv)
{
	if(argc != 3)
	{
		std::cerr << "Usage: main <port> <catalog>\n";
		return 1;
	}
	int port;
	sscanf(argv[1], "%d", &port);

	std::string catalog(argv[2]);

	Server server(port, catalog);
	server.run();
	return 0;
}

