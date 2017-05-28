#include "ClientHandler.h"

void ClientHandler::closeConnection()
{
	if(close(sockfd) < 0)
	{
		std::cerr << "Error: " << strerror(errno);
		throw new std::runtime_error("Closing connection exception");
	}
}

HttpMsg ClientHandler::getHttpRequest()
{
	struct timeval tv;
	tv.tv_sec = inactiveTimeout; 
	tv.tv_usec = 0;

	fd_set descriptors;
	FD_ZERO(&descriptors);
	FD_SET(sockfd, &descriptors);

	int ready = select(sockfd+1, &descriptors, NULL, NULL, &tv);
	if(ready < 0)
	{
		std::cerr << "Error: " << strerror(errno);
		throw new std::runtime_error("Select exception");
	}
	else if(ready == 0)
		return HttpMsg();

	constexpr size_t bufferSize = 4096;
	char buffer[bufferSize];
	ssize_t readBytes = recv(sockfd, buffer, bufferSize, 0);
	if(readBytes < 0)
	{
		std::cerr << "Error: " << strerror(errno);
		throw new std::runtime_error("recv exception");
	}
	if(readBytes == 0)
	{
		return HttpMsg();
	}
	return HttpMsg(buffer, readBytes);
}

HttpMsg ClientHandler::getHttpResponse(const HttpMsg& request)
{
	std::cout << request << '\n';
	std::string response = "";
	const std::map<std::string, std::string>& header = request.getHeader();
	if(request.getHttpMethod() != HttpMsg::HttpMethod::GET)
	{
		return get501Html();
	}
	else
	{
		std::string hostName = header.at("Host");
		size_t portBegin = hostName.find_last_of(":");
		hostName = hostName.substr(0, portBegin);
		std::string path = catalog + "/" + hostName + header.at("what");
		
		if(utils::isCatalog(path))
		{
			if(path.back() != '/')
				path += "/";
			std::cout << "Catalog\n";
			path += "index.html";
		}
		if(!utils::isSafe(path))
		{
			std::cerr << "Path not safe\n";
			return get404Html();//getBadUserHtml();
		}
		std::ifstream reqFile(path, std::ifstream::binary);
		if(!reqFile.is_open())
		{
			std::cerr << "File not found\n";
			std::cout << path << '\n';
			return get404Html();
		}
		reqFile.seekg(0, reqFile.end);
		size_t fileLen = reqFile.tellg();
		reqFile.seekg(0, reqFile.beg);
		if(fileLen >= 10000000)
		{
			reqFile.close();
			return get404Html();
		}

		
		response += "HTTP/1.1 200 OK\r\n";
		response += "Content-Type: " + utils::getMime(path) + "\r\n";
		response += "Content-Length: " + std::to_string(fileLen) + "\r\n";
		response += "Connection: keep-alive\r\n\r\n";

		size_t bufferSize = response.length() + fileLen;
		char* buffer = new char[bufferSize];
		std::memcpy(buffer, response.c_str(), response.length());
		reqFile.read(&buffer[response.length()], fileLen);
		reqFile.close();
		HttpMsg httpResponse(buffer, bufferSize);
		delete[] buffer;

		return httpResponse;

	}
	//std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nConnection: keep-alive\r\nContent-Length: 92\r\n\r\n";

	
}

void ClientHandler::sendMsg(const HttpMsg& msg)
{
	const std::map<std::string, std::string>& header = msg.getHeader();

	//std::cout << msg << '\n';

	if(header.at("method") != "RESPONSE")
	{
		std::cerr << "Msg is not an response\n";
		throw std::runtime_error("Response exception");
	}

	std::string res = "HTTP/1.1 " + 
		header.at("response-code") + 
		" " + 
		header.at("response-phrase") +
		"\r\n";

	for(const auto& it : header)
	{
		if(it.first == "method" || it.first == "response-code" || it.first == "response-phrase")
			continue;
		res += it.first + ": " + it.second + "\r\n";
	}


	res += "\r\n";
	size_t bufferSize = res.length();
	res += std::string(msg.getData().get(), msg.getDataSize());
	bufferSize += msg.getDataSize();
	ssize_t bytesSent = send(sockfd, res.c_str(), bufferSize, 0);

	if(bytesSent < 0)
	{
		std::cerr << "Error: " << strerror(errno);
		throw std::runtime_error("Send exception");
	}
	else if((size_t)bytesSent < bufferSize)
	{
		std::cout << "not enough bytes sent\n";
	}
}

void ClientHandler::run()
{
	while(true)
	{
		HttpMsg request = getHttpRequest();

		if(request.empty())
		{
			closeConnection();
			return;
		}
		HttpMsg response = getHttpResponse(request);
		sendMsg(response);
	}
}

HttpMsg ClientHandler::get404Html() const
{
	std::ifstream reqFile(catalog + "/_PRIVATE__/404.html", std::ifstream::binary);
	if(!reqFile.is_open())
	{
		std::cerr << "Error while opening 404\n";
		throw new std::runtime_error("opening 404 exception");
	}
	reqFile.seekg(0, reqFile.end);
	size_t fileSize = reqFile.tellg();
	reqFile.seekg(0, reqFile.beg);

	std::string response = "";
	response += "HTTP/1.1 404 NOT FOUND\r\n";
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + std::to_string(fileSize) + "\r\n";
	response += "Connection: keep-alive\r\n\r\n";
	size_t bufferSize = fileSize + response.length();
	char* buffer = new char[bufferSize];
	std::memcpy(buffer, response.c_str(), response.length());
	reqFile.read(&buffer[response.length()], fileSize);
	reqFile.close();

	HttpMsg httpResponse(buffer, bufferSize);
	delete[] buffer;
	return httpResponse;
}

HttpMsg ClientHandler::get501Html() const
{
	std::ifstream reqFile(catalog + "/_PRIVATE__/501.html", std::ifstream::binary);
	if(!reqFile.is_open())
	{
		std::cerr << "Error while opening 501\n";
		throw new std::runtime_error("opening 501 exception");
	}
	reqFile.seekg(0, reqFile.end);
	size_t fileSize = reqFile.tellg();
	reqFile.seekg(0, reqFile.beg);

	std::string response = "";
	response += "HTTP/1.1 501 NOT IMPLEMENTED\r\n";
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + std::to_string(fileSize) + "\r\n";
	response += "Connection: keep-alive\r\n\r\n";
	size_t bufferSize = fileSize + response.length();
	char* buffer = new char[bufferSize];
	std::memcpy(buffer, response.c_str(), response.length());
	reqFile.read(&buffer[response.length()], fileSize);
	reqFile.close();
	HttpMsg httpResponse(buffer, bufferSize);
	delete[] buffer;
	return httpResponse;
}