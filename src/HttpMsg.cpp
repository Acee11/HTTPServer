#include "HttpMsg.h"
HttpMsg::~HttpMsg()
{ }

HttpMsg::HttpMsg() :
header(std::map<std::string, std::string>()),
data(nullptr),
dataSize(0)
{ }

HttpMsg::HttpMsg(const HttpMsg& msg)
{
	*this = msg;
}
HttpMsg::HttpMsg(HttpMsg&& msg)
{
	*this = msg;
}


HttpMsg& HttpMsg::operator = (const HttpMsg& msg)
{
	dataSize = msg.dataSize;
	data.reset(new char[dataSize]);
	std::memcpy(data.get(), msg.data.get(), msg.dataSize * sizeof(char));
	header = msg.header;

	return *this;
}

HttpMsg& HttpMsg::operator = (HttpMsg&& msg)
{
	dataSize = msg.dataSize;
	data = std::move(msg.data);
	msg.dataSize = 0;
	header = std::move(msg.header);

	return *this;
}

HttpMsg::HttpMsg(const char* rawData, size_t len)
{

	header = std::map<std::string, std::string>();
	data = nullptr;
	dataSize = 0;
	
	size_t offset = 0;

	const char* p = std::find(&rawData[offset], &rawData[len-1], ' ');
	size_t foundIdx = (size_t)(p - rawData);

	if(foundIdx == len)
		return;
	//std::cout << foundIdx << ' ' << offset << ' ' << len << '\n';
	std::string s(&rawData[offset], foundIdx - offset);
	offset = foundIdx+1;
	if(s == "HTTP/1.1")
	{
		header["method"] = "RESPONSE";
		p = std::find(&rawData[offset], &rawData[len-1], ' ');
		foundIdx = (size_t)(p - rawData);
		if(foundIdx == len)
			return;
		s = std::string(&rawData[offset], foundIdx - offset);
		offset = foundIdx+1;
		header["response-code"] = std::move(s);

		p = std::find(&rawData[offset], &rawData[len-1], '\r');
		foundIdx = (size_t)(p - rawData);
		if(foundIdx == len)
			return;
		s = std::string(&rawData[offset], foundIdx - offset);
		offset = foundIdx+2;
		header["response-phrase"] = std::move(s);
	}
	else
	{
		header["method"] = std::move(s);

		p = std::find(&rawData[offset], &rawData[len-1], ' ');
		foundIdx = (size_t)(p - rawData);
		if(foundIdx == len)
			return;
		s = std::string(&rawData[offset], foundIdx - offset);
		offset = foundIdx+1;
		header["what"] = std::move(s);

		p = std::find(&rawData[offset], &rawData[len-1], '\n');
		foundIdx = (size_t)(p - rawData);
		if(foundIdx == len)
			return;
		offset = foundIdx+1;
	}



	std::string d;

	while(true)
	{
		if(offset >= len)
			break;

		p = std::find(&rawData[offset], &rawData[len-1], '\r');
		size_t endOfLine = (size_t)(p - rawData);
		if(endOfLine >= len-1 || rawData[endOfLine+1] != '\n')
		{
			std::cerr << "Wrong format(1)\n";
			throw new std::runtime_error("wrong format");
		}
		if(offset == endOfLine)
		{
			offset += 2;
			dataSize = len - offset;
			data.reset(new char[dataSize]);
			std::memcpy(data.get(), &rawData[offset], dataSize * sizeof(char));
			return;
		}


		p = std::find(&rawData[offset], &rawData[endOfLine], ':');
		foundIdx = (size_t)(p - rawData);
		if(foundIdx >= endOfLine)
		{
			std::cerr << "Wrong format(2)\n";
			throw new std::runtime_error("wrong format");
		}

		s = std::string(&rawData[offset], foundIdx - offset);
		offset = foundIdx + 1;
		while(rawData[offset] == ' ')
			++offset;
		d = std::string(&rawData[offset], endOfLine - offset);
		offset = endOfLine + 2;
		header[std::move(s)] = std::move(d);
	}


}

const std::map<std::string, std::string>& HttpMsg::getHeader() const
{
	return header;
}

const std::unique_ptr<char[]>& HttpMsg::getData() const
{
	return data;
}

size_t HttpMsg::getDataSize() const
{
	return dataSize;
}


HttpMsg::HttpMethod HttpMsg::getHttpMethod() const
{
	auto it = header.find("method");
	if(it == header.end())
		return HttpMethod::NONE;
	const std::string& methodStr = (*it).second;

	if(methodStr == "GET")
		return HttpMethod::GET;
	else if (methodStr == "HEAD")
		return HttpMethod::HEAD;
	else if (methodStr == "PUT")
		return HttpMethod::PUT;
	else if (methodStr == "POST")
		return HttpMethod::POST;
	else if (methodStr == "DELETE")
		return HttpMethod::DELETE;
	else if (methodStr == "OPTIONS")
		return HttpMethod::OPTIONS;
	else if (methodStr == "TRACE")
		return HttpMethod::TRACE;
	else if (methodStr == "CONNECT")
		return HttpMethod::CONNECT;
	else if (methodStr == "PATCH")
		return HttpMethod::PATCH;
	else if (methodStr == "RESPONSE")
		return HttpMethod::RESPONSE;
	else
		return HttpMethod::NONE;
}

bool HttpMsg::empty()
{
	return header.empty();
}

std::ostream & operator << (std::ostream& stream, const HttpMsg& msg)
{
	for(const auto& it : msg.header)
		stream << it.first << ':' << it.second << '\n';

	return stream;
}