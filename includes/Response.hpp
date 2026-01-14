#pragma once

#include "Config.hpp"
#include "Server.hpp"
#include "webserv.hpp"

class ServerConfig;
class Location;
class Server;
class ClientState;

class ParseURL
{
public:
	std::string path_script;
	std::string path_info;
	std::string query_string;
	std::string url;
	std::string loc_left;
	std::string loc_right;
};

class Response
{
public:
	std::string method;
	std::string url;
	std::string version;
	std::map<std::string, std::string> header;
	std::string body;
};

Response parseRequest(const std::string &request);
std::string getPath(const std::string &url, const ServerConfig &server, Location &location);
std::string getRequest(const Response &rep, const ServerConfig &server, Server &srv, ClientState &client_state);

std::string handleGET(const std::string &path, const ServerConfig &server, const Location &loc, const ParseURL &parsed_url);
std::string handlePOST(const Response &rep, const ServerConfig &server);
std::string handleDELETE(const Response &rep, const ServerConfig &server, Location &loc);
std::string handleCGI(const Response &rep, const ServerConfig &server, std::string path, const Location &loc, const ParseURL &parsed_url, Server &srv, ClientState &client_state);
