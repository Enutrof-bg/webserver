#pragma once

#include "Config.hpp"
#include "Response.hpp"
#include "webserv.hpp"

class ServerConfig;

std::string ft_handling_error(const ServerConfig &server, int code);