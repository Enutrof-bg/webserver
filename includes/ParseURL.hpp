#pragma once

// #include "webserv.hpp"
// #include "Response.hpp"
#include <iostream>
#include <string>
#include <vector>

#include "Config.hpp"

class Response;
struct ServerConfig;

class ParseURL
{
public:
	std::string path_script;
	std::string path_info;
	std::string query_string;
	std::string url;
	std::string loc_left;
	std::string loc_right;

public:
	static ParseURL ft_parseURL(const Response &rep, const ServerConfig &server);
};
