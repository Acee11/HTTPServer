#ifndef CLIENT_H_
#define CLIENT_H_

#include <iostream>
#include <fstream>  
#include <exception>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "HttpMsg.h"
#include "utils.h"

class ClientHandler
{
private:
	int sockfd;
	int inactiveTimeout;
	std::string catalog;

	void closeConnection();
	HttpMsg getHttpRequest();
	HttpMsg getHttpResponse(const HttpMsg& httpRequest);
	void sendMsg(const HttpMsg& msg);

	HttpMsg getBadUserHtml() const;
	HttpMsg get404Html() const;
	HttpMsg get501Html() const;

	//static const std::map<std::string, std::string> mimeTranslator;
	//static std::map<std::string, std::string> createMimeTranslator();


	//static std::string getMime(const std::string& path);
	//static bool isSafe(const std::string& path);
	//static bool isCatalog(const std::string& path);


public:
	ClientHandler(int sockfd, int inactiveTimeout, const std::string& catalog) :
	sockfd(sockfd),
	inactiveTimeout(inactiveTimeout),
	catalog(catalog)
	{ }
	
	void run();
};

#endif