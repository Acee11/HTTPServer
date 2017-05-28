#ifndef UTILS_H_
#define UTILS_H_

#include <map>
#include <stddef.h>
#include <sys/stat.h>

namespace utils
{

	bool isSafe(const std::string& path);
	bool isCatalog(const std::string& path);
	
	std::string getMime(const std::string& path);
	const std::map<std::string, std::string> mimeTranslator = {
		{"html", "text/html"},
		{"css", "text/css"},
		{"txt", "text/plain"},
		{"jpg", "image/jpeg"},
		{"jpeg", "image/jpeg"},
		{"png", "image/png"},
		{"pdf", "application/pdf"}
	};

}

#endif