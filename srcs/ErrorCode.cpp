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

	std::cout << "Handling error code: " << code << std::endl;
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

std::string ft_get_default_body(int code, const std::string &new_url)
{
	(void)new_url;
	switch (code)
	{
	case 201:
		return "<h1>201 Created</h1><p>The resource has been created.</p>";
		break;

	case 301:
		return "<h1>301 Moved Permanently</h1><p>The document has moved.</p>";
		break;
	
	case 302:
		return "<h1>302 Moved Temporarily</h1><p>The document has moved temporarily.</p>";
		break;

	default:
		return "";
		break;
	}
}

std::string ft_get_default_header(int code, const std::string &body, const std::string &new_url)
{
	std::stringstream return_ss;

	std::stringstream ss;
	ss << body.length();
	switch (code)
	{
	case 200:
		return_ss << "HTTP/1.1 200 OK\r\n"
				<< "Content-Type: " << new_url << "; charset=UTF-8\r\n"
				<< "Content-Length: " << ss.str() << "\r\n"
				<< "Connection: close\r\n"
				<< "\r\n\r\n";
		return return_ss.str();
		break;

	case 201:
		return_ss << "HTTP/1.1 201 Created\r\n"
				<< "Content-Type: text/html; charset=UTF-8\r\n"
				<< "Content-Length: " << ss.str() << "\r\n"
				<< "Location: " << new_url << "\r\n"
				<< "Connection: close\r\n"
				<< "\r\n\r\n";
		return return_ss.str();
		break;

	case 204:
		return_ss << "HTTP/1.1 204 No Content\r\n"
				<< "Content-Length: 0\r\n"
				<< "Connection: close\r\n"
				<< "\r\n\r\n";
		return return_ss.str();
		break;

	case 301:
		return_ss << "HTTP/1.1 301 Moved Permanently\r\n"
				<< "Location: " << new_url << "\r\n"
				<< "Content-Type: text/html; charset=UTF-8\r\n"
				<< "Content-Length: " << ss.str() << "\r\n"
				<< "Connection: close\r\n"
				<< "\r\n\r\n";
		return return_ss.str();
		break;

	case 302:
		return_ss << "HTTP/1.1 302 Moved Temporarily\r\n"
				<< "Location: " << new_url << "\r\n"
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

std::string ft_move_code(const ServerConfig &server, int code, std::string &path)
{
	std::string body;
	std::string header;
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
		}
	}
	if (body.empty())
	{
		body = ft_get_default_body(code, path);
	}
	header = ft_get_default_header(code, body, path);
	return (header + body);
}

std::string ft_serve_file(const ServerConfig &server, const std::string &path, int code, const std::string &content_type)
{
	std::ifstream file(path.c_str(), std::ios::binary);
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	if (content.empty())
	{
		std::cerr << "Error reading file: " << path << std::endl;
		return ft_handling_error(server, 404);
	}

	// std::string temp_content_type = ft_get_extension_file(path);
	std::string header = ft_get_default_header(code, content, content_type);
	return (header + content);
}
std::string ft_serve_no_body(const ServerConfig &server, int code, const std::string &content_type)
{
	(void)server;
	(void)content_type;
	std::string body = "";
	std::string header = ft_get_default_header(code, body, content_type);
	return (header + body);
}

std::string ft_generate_autoindex_body(const ServerConfig &server, const std::string &dir_path, const std::string &url)
{
	(void)server;
	std::ostringstream body;
	DIR *dir = opendir(dir_path.c_str());
	if (dir != NULL)
	{
		body << "<!DOCTYPE html>\n"
				<< "<html>\n<head><title>Index of " << url << "</title></head>\n"
				<< "<body>\n"
				<< "<h1>Index of " << url << "</h1>\n"
				<< "<ul>\n";

		struct dirent *ent;
		while ((ent = readdir(dir)) != NULL)
		{
			std::string filename = ent->d_name;
			if (filename == ".")
				continue;
			body << "<li><a href=\"" << url;
			if (!url.empty() && url[url.length() - 1] != '/')
				body << "/";
			body << filename << "\">" << filename << "</a></li>\n";
		}
		closedir(dir);

		body << "</ul>\n</body>\n</html>\n";
		return body.str();
	}
	else
	{
		// std::cerr << "Failed to open directory for autoindex: " << dir_path << std::endl;
		// return ft_handling_error(server, 404);
		return "";
	}
}

std::string ft_generate_autoindex_page(const ServerConfig &server, int code, const std::string &body)
{
	(void)server;
	(void)code;
	(void)body;
	if (body.empty())
	{
		return ft_handling_error(server, 404);
	}
	std::string header = ft_get_default_header(code, body, "text/html");

	return (header + body);
}