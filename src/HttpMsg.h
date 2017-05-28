#ifndef HTTP_MSG_H_
#define HTTP_MSG_H_

#include <iostream>
#include <memory>
#include <map>
#include <algorithm>
#include <cstring>


class HttpMsg
{
private:
	std::map<std::string, std::string> header;
	//char* data;
	std::unique_ptr<char[]> data;
	size_t dataSize;
public:
	enum HttpMethod 
	{
		NONE, GET, HEAD, PUT, POST, DELETE, OPTIONS, TRACE, CONNECT, PATCH, RESPONSE
	};

	~HttpMsg();
	HttpMsg();
	HttpMsg(const HttpMsg& msg);
	HttpMsg(HttpMsg&& msg);
	HttpMsg(const char* rawData, size_t len);
	HttpMsg& operator = (const HttpMsg& msg);
	HttpMsg& operator = (HttpMsg&& msg);

	char* unparseHttpMsg();

	const std::map<std::string, std::string>& getHeader() const;
	const std::unique_ptr<char[]>& getData() const;
	size_t getDataSize() const;
	HttpMethod getHttpMethod() const;


	bool empty();

	friend std::ostream& operator << (std::ostream& stream, const HttpMsg& msg);


};

#endif