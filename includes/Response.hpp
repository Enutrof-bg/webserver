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
	ParseURL parsed_url;
};

Response parseRequest(const std::string &request);
Location getLocation(const std::string &url, const ServerConfig &server);
ParseURL parseURL(const Location &location);
std::string ft_redirection(const ServerConfig &server, const ParseURL &parsed_url);
int ft_check_method(const Location &loc, const Response &rep);
std::string ft_check_body_size(const Response &rep, const ServerConfig &server, const Location &loc);

std::string getPath(const std::string &url, const ServerConfig &server, Location &location);
std::string getRequest(Response &rep, const ServerConfig &server, Server &srv, ClientState &client_state);

std::string handleGET(const std::string &path, const ServerConfig &server, const Location &loc, const ParseURL &parsed_url);
std::string handlePOST(const Response &rep, const ServerConfig &server);
std::string handleDELETE(const Response &rep, const ServerConfig &server, Location &loc);
std::string handleCGI(const Response &rep, const ServerConfig &server, std::string path, const Location &loc, const ParseURL &parsed_url, Server &srv, ClientState &client_state);

char **ft_return_cgi_env(const Response &rep, const ServerConfig &server,
						std::string path, const Location &loc, const ParseURL &parsed_url);