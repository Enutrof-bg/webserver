#pragma once

#include "Config.hpp"
#include "Response.hpp"
#include "webserv.hpp"

class ServerConfig;
// class ErrorCode
// {

// };

std::string ft_handling_error(const ServerConfig &server, int code);