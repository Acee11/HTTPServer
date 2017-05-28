#include "Server.h"

Server::Server()
{
	this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(this->sockfd < 0)
	{
		std::cerr << "Failed to create a socket\n";
		throw new std::runtime_error("Failed to create a socket");
	}


	sockaddr_in serverAddress;
	bzero(&serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(80);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(this->sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
	{
		std::cerr << "Failed to bind a socket to address INADDR_ANY and port 80\n";
		throw new std::runtime_error("Binding a socket exception");
	}

	if (listen(this->sockfd, 64) < 0)
	{
		std::cerr << "Error while listening\n";
		throw new std::runtime_error("Listening exception");
	}
}

Server::Server(int port, const std::string& catalog) :
catalog(catalog)
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(this->sockfd < 0)
	{
		std::cerr << "Failed to create a socket\n";
		throw new std::runtime_error("Creating a socket exception");
	}


	sockaddr_in serverAddress;
	bzero(&serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(this->sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
	{
		std::cerr << "Failed to bind a socket to address INADDR_ANY and port " << port << '\n';;
		throw new std::runtime_error("Binding a socket exception");
	}

	if (listen(this->sockfd, 64) < 0)
	{
		std::cerr << "Error while listening\n";
		throw new std::runtime_error("Listening exception");
	}
}

void Server::handleClient(int clientConnSockfd)
{
	ClientHandler clientHandler(clientConnSockfd, DEFAULT_TIME_LEFT, catalog);
	clientHandler.run();
}

int Server::waitForClient()
{
	sockaddr_in unusedClientAddress;
	socklen_t unusedClientAddressLen = sizeof(unusedClientAddress);
	int clientConnSockfd = accept(
		sockfd, 
		(struct sockaddr*)&unusedClientAddress, 
		&unusedClientAddressLen
	);
    if(clientConnSockfd < 0)
    {
    	std::cerr << strerror(errno) << '\n';
    	throw new std::runtime_error("Waiting for client exception");
    }
    return clientConnSockfd;
}

void Server::run()
{
	while(true)
	{
		int clientConnSockfd = waitForClient();
		std::thread t1(&Server::handleClient, this, clientConnSockfd);
		t1.detach();
	}
	
}






