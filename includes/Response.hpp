#pragma once

#include "Config.hpp"
#include "webserv.hpp"

struct Response
{
	std::string method;
	std::string url;
	std::string version;
	std::map<std::string, std::string> header;
	std::string body;
};

Response parseRequest(const std::string &request);
// {
// 	Response rep;
// 	std::istringstream stream(request);
// 	std::string line;



// 	return rep;
// }