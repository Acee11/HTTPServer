#ifndef SERVER_H_
#define SERVER_H_
#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <strings.h>
#include <exception>
#include <memory>
#include <map>
#include <thread>
#include "ClientHandler.h"

#define DEFAULT_TIME_LEFT 5

class Server
{
private:
	int sockfd;
	std::string catalog;
	int waitForClient();
	void handleClient(int clientConnSockfd);
public:
	Server();
	Server(int port, const std::string& catalog);
	void run();
};

#endif