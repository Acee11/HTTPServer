#include "utils.h"



bool utils::isSafe(const std::string& path)
{
	size_t exploitCheck = path.find("..");
	if(exploitCheck != std::string::npos)
		return false;

	exploitCheck = path.find("_PRIVATE__");
	if(exploitCheck != std::string::npos)
		return false;

	return true;
}

bool utils::isCatalog(const std::string& path)
{
	FILE *fp = fopen(path.c_str(), "r");
	if(!fp)
		return false;
	struct stat fileInfo;
	fstat(fileno(fp), &fileInfo);
	if (!S_ISREG(fileInfo.st_mode)) 
	{
	    fclose(fp);
	    return true;
	}
	fclose(fp);
	return false;

}

std::string utils::getMime(const std::string& path)
{
	size_t dotLocation = path.find_last_of(".");
	if(dotLocation == std::string::npos)
		return "application/octet-stream";
	std::string extension = path.substr(dotLocation+1);

	std::string result;
	try
	{
		result = mimeTranslator.at(extension);
	}
	catch(const std::exception& e)
	{
		result = "application/octet-stream";
	}
	return result;
}







