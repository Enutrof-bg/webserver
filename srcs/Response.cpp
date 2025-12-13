#include "../includes/Response.hpp"

Response parseRequest(const std::string &request)
{
	Response rep;
	std::istringstream stream(request);
	std::string line;

	std::getline(stream, line);
	std::cout << "[" << line << "]" << std::endl;
	std::cout << "TESTCACA" << std::endl;

	std::istringstream temp(line);
	temp >> rep.method >> rep.url >> rep.version;

	std::cout << "Method:" << rep.method << std::endl;
	std::cout << "URL:" << rep.url << std::endl;
	std::cout << "VERSION:" << rep.version << std::endl;

	while (std::getline(stream, line) && line != "/r")
	{
		size_t pos = line.find(':');
		if (pos != std::string::npos)
		{
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 2);
			rep.header[key] = value;
		}
	}
	std::cout <<"HEADER" << std::endl;
	std::map<std::string, std::string>::iterator it;
	for (it = rep.header.begin(); it != rep.header.end(); it++)
	{
		std::cout << "First: " <<it->first << " | "<<"Second: "<<it->second << std::endl;
	}
	while (std::getline(stream ,line))
	{
		rep.body += line;
	}
	std::cout << "BODY" << std::endl;
	std::cout << rep.body << std::endl;
	return rep;
}