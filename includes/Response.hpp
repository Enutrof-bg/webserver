#pragma once

#include "Config.hpp"
#include "Server.hpp"
#include "webserv.hpp"

struct ServerConfig;
struct Location;

struct ParseURL
{
	std::string path_script;
	std::string path_info;
	std::string query_string;
	std::string url;
};

struct Response
{
	std::string method;
	std::string url;
	std::string version;
	std::map<std::string, std::string> header;
	std::string body;
};

Response parseRequest(const std::string &request);
std::string getPath(const std::string &url, const ServerConfig &server);
std::string getRequest(const Response &rep, const ServerConfig &server);

std::string handleGET(const std::string &path, const ServerConfig &server);
std::string handlePOST(const Response &rep, const ServerConfig &server);
std::string handleDELETE(const Response &rep, const ServerConfig &server);
std::string handleCGI(const Response &rep, const ServerConfig &server, std::string path, const Location &loc, const ParseURL &parsed_url);

// {
// 	Response rep;
// 	std::istringstream stream(request);
// 	std::string line;



// 	return rep;
// }