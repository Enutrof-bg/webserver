#include "../includes/ErrorCode.hpp"
#include <sstream>

std::string ft_get_default_error_header(int code, const std::string &body)
{
	std::stringstream ss;
	ss << body.length();
	switch (code)
	{
	case 404:
		return "HTTP/1.1 404 Not Found\r\nContent-Length: " + ss.str() + "\r\n\r\n";
		break;
	
	case 405:
		return "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
		break;

	case 413:
		return "HTTP/1.1 413 Payload Too Large\r\n\r\n";
		break;

	default:
		return "";
		break;
	}
}

std::string ft_get_default_error_body(int code)
{
	switch (code)
	{
	case 404:
		return "<h1>ERROR 404 Not found</h1><ap><a title=\"GO BACK\" href=\"/\">go back</a>";
		break;
	case 405:
		return "<h1>ERROR 405 Method Not Allowed</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
		break;
	case 413:
		return "<h1>ERROR 413 Payload Too Large</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
		break;

	default:
		return "";
		break;
	}
}

std::string ft_handling_error(const ServerConfig &server, int code)
{
	std::string header;
	std::string body;
	std::map<int, std::string>::const_iterator it = server._config_error_page.find(code);

	if (it != server._config_error_page.end() && !it->second.empty())
	{
		std::string path_error = it->second;
		std::cout << "Page erreur:" << path_error << std::endl;
		
		std::ifstream error_file(path_error.c_str(), std::ios::binary);
		if (error_file.is_open())
		{
			body = std::string((std::istreambuf_iterator<char>(error_file)), 
								std::istreambuf_iterator<char>());
			error_file.close();
			// std::cout << body << std::endl;
		}
	}
	if (body.empty())
	{
		body = ft_get_default_error_body(code);
	}
	header = ft_get_default_error_header(code, body);

	return (header + body);
}