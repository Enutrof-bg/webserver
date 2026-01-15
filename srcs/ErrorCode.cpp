#include "../includes/ErrorCode.hpp"


std::string ft_get_default_error_header(int code, const std::string &body)
{
	std::stringstream return_ss;

	std::stringstream ss;
	ss << body.length();
	switch (code)
	{
	case 400:
		return_ss << "HTTP/1.1 400 Bad Request\r\n"
				<< "Content-Type: text/html; charset=UTF-8\r\n"
				<< "Content-Length: " << ss.str() << "\r\n"
				<< "Connection: close\r\n"
				<< "\r\n\r\n";
		return return_ss.str();
		break;

	case 403:
		return_ss << "HTTP/1.1 403 Forbidden\r\n"
				<< "Content-Type: text/html; charset=UTF-8\r\n"
				<< "Content-Length: " << ss.str() << "\r\n"
				<< "Connection: close\r\n"
				<< "\r\n\r\n";
		return return_ss.str();
		break;

	case 404:
		return_ss << "HTTP/1.1 404 Not Found\r\n"
				<< "Content-Type: text/html; charset=UTF-8\r\n"
				<< "Content-Length: " << ss.str() << "\r\n"
				<< "Connection: close\r\n"
				<< "\r\n\r\n";
		return return_ss.str();
		break;
	
	case 405:
		return_ss << "HTTP/1.1 405 Method Not Allowed\r\n"
				<< "Content-Type: text/html; charset=UTF-8\r\n"
				<< "Content-Length: " << ss.str() << "\r\n"
				<< "Connection: close\r\n"
				<< "\r\n\r\n";
		return return_ss.str();
		break;

	case 413:
		return_ss << "HTTP/1.1 413 Payload Too Large\r\n"
				<< "Content-Type: text/html; charset=UTF-8\r\n"
				<< "Content-Length: " << ss.str() << "\r\n"
				<< "Connection: close\r\n"
				<< "\r\n\r\n";
		return return_ss.str();
		break;

	case 500: 
		return_ss << "HTTP/1.1 500 Internal Server Error\r\n"
				<< "Content-Type: text/html; charset=UTF-8\r\n"
				<< "Content-Length: " << ss.str() << "\r\n"
				<< "Connection: close\r\n"
				<< "\r\n\r\n";
		return return_ss.str();
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
	case 400:
		return "<h1>ERROR 400 Bad Request</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
		break;
		
	case 403:
		return "<h1>ERROR 403 Forbidden</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
		break;

	case 404:
		return "<h1>ERROR 404 Not found</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
		break;

	case 405:
		return "<h1>ERROR 405 Method Not Allowed</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
		break;

	case 413:
		return "<h1>ERROR 413 Payload Too Large</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
		break;

	case 500: 
		return "<h1>ERROR 500 Internal Server Error</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
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