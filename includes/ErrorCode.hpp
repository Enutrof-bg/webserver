#pragma once

#include "Config.hpp"
#include "Response.hpp"
#include "webserv.hpp"

struct ServerConfig;

std::string ft_handling_error(const ServerConfig &server, int code);
std::string ft_move_code(const ServerConfig &server, int code, std::string &path);
std::string ft_serve_file(const ServerConfig &server, const std::string &path, int code, const std::string &content_type);
std::string ft_serve_no_body(const ServerConfig &server, int code, const std::string &content_type);
std::string ft_generate_autoindex_body(const ServerConfig &server, const std::string &dir_path, const std::string &url);
std::string ft_generate_autoindex_page(const ServerConfig &server, int code, const std::string &body);

std::string ft_generate_form_page(const ServerConfig &server, int code, const std::string &body, const std::string &new_url,std::map<std::string, std::string> data);