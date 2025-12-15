#include "../includes/Response.hpp"

Response parseRequest(const std::string &request)
{
	Response rep;
	std::istringstream stream(request);
	std::string line;

	std::getline(stream, line);
	std::cout << "[" << line << "]" << std::endl;
	std::cout << "TESTCACA" << std::endl;

	std::istringstream temp(line);
	temp >> rep.method >> rep.url >> rep.version;

	std::cout << "Method:" << rep.method << std::endl;
	std::cout << "URL:" << rep.url << std::endl;
	std::cout << "VERSION:" << rep.version << std::endl;

	while (std::getline(stream, line))
	{
		if (!line.empty() && line[line.length()-1] == '\r')
			line.erase(line.length()-1);
		
		if (line.empty())
			break;

		size_t pos = line.find(':');
		if (pos != std::string::npos)
		{
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 2);
			rep.header[key] = value;
		}
	}
	std::cout <<"HEADER" << std::endl;
	std::map<std::string, std::string>::iterator it;
	for (it = rep.header.begin(); it != rep.header.end(); it++)
	{
		std::cout << "First: " <<it->first << " | "<<"Second: "<<it->second << std::endl;
	}
	while (std::getline(stream ,line))
	{
		rep.body += line;
	}
	std::cout << "BODY" << std::endl;
	std::cout << rep.body << std::endl;
	return rep;
}

std::string getPath(const std::string &url, const ServerConfig &server)
{
	// return "asd";
	if (url.empty())
		std::cout << "ERREUR" << std::endl;
	std::cout << url << std::endl;
	// std::cout << path << std::endl;
	std::string path = server._config_root;

	std::cout << path << std::endl;

	if (url == "/" || url[url.length() - 1] == '/')
	{
		path = path + url + server._config_index;
	}
	else
	{
		path = path + url;
	}
	return path;
}

std::string getRequest(const Response &rep, const ServerConfig &server)
{
	//error a gerer check method
	// (void)server;
	// return "caca1";
	// std::cout << rep.url << std::endl;
	std::string path = getPath(rep.url, server);
	// return "caca";
	if (rep.method == "GET")
	{
		return handleGET(path, server);
	}
	else if (rep.method == "POST")
	{
		return handlePOST(rep, server);
	}
	return "test";
}
std::string handleGET(const std::string &path, const ServerConfig &server)
{
	(void)server;
	std::cout << "Chemin apth: " << path << std::endl;
	
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "Fichier not found: " << path << std::endl;
		
		std::map<int, std::string>::const_iterator it = server._config_error_page.find(404);
		std::string body;
		
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

				std::cout << "--------------asdasd----" << std::endl;
				std::cout << body << std::endl;
			}
		}
		
	
		if (body.empty())
		{
			//default page si error404.html nest pas defini
			body = 
				"<!DOCTYPE html>\n"
				"<html>\n"
				"<head><title>404 Not Found</title></head>\n"
				"<body>\n"
				"<h1>404 Not Found</h1>\n"
				"<p>Le fichier demandé n'existe pas.</p>\n"
				"</body>\n"
				"</html>\n";
		}
		
		std::ostringstream response;
		response << "HTTP/1.1 404 Not Found\r\n"
				 << "Content-Type: text/html; charset=UTF-8\r\n"
				 << "Content-Length: " << body.length() << "\r\n"
				 << "Connection: close\r\n"
				 << "\r\n"
				 << body;
		
		return response.str();
	}
	//protec

	//a changer
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	std::ostringstream response;
	response << "HTTP/1.1 200 OK\r\n"
			<<"Content-Type: text/html; charset=UTF-8\r\n"
			<<"Content-Length: " << content.length()<<"\r\n"
			<<"Connection: close\r\n"
			<<"\r\n"
			<< content;

	return (response.str());
}

std::string handlePOST(const Response &rep, const ServerConfig &server)
{
	(void)rep;
	(void)server;
	std::cout << "----------POST_BODY-------" <<std::endl;
	std::cout << rep.body  << std::endl;
	//check rep.length or error413
	std::map<std::string, std::string>::const_iterator it = rep.header.find("Content-Length");
	if (it != rep.header.end())
	{
		size_t content_length = atoi(it->second.c_str());
		std::cout << "content_length:"<< content_length << std::endl;
	}
	// for (it = rep.header.begin(); it != rep.header.end(); it++)
	// {

	// }
	//parsing body
	std::istringstream stream(rep.body);
	std::string line;
	std::map<std::string, std::string> data;
	while (std::getline(stream, line, '&'))
	{
		std::cout << line << std::endl;
		size_t pos = line.find('=');
		if (pos != std::string::npos)
		{
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);
			data[key] = value;
		}
	}
	//create html 
	std::ostringstream newbody;
	newbody << "<!DOCTYPE html>\n"
		<< "<html>\n<head><title>POST reçu</title></head>\n"
		<< "<body>\n"
		<< "<h1>Données reçues</h1>\n"
		<< "<ul>\n";

	for (std::map<std::string, std::string>::iterator it = data.begin();
		it != data.end(); it++)
		{
			newbody << "<li><b>" << it->first << "</b>:" << it->second << "</li>\n";
		}
		newbody << "</ul>\n"
		<< "<p>Body brut: <code>" << rep.body << "</code></p>\n"
		<< "<br><p><a title=\"Motherfucking Website\" href=\"index.html\">go back</a></p></br>\n"
		<< "</body>\n</html>\n";

	std::ostringstream response;
	response << "HTTP/1.1 200 OK\r\n"
			<< "Content-Type: text/html; charset=UTF-8\r\n"
			<< "Content-Length: " << newbody.str().length() << "\r\n"
			<< "Connection: close\r\n"
			<< "\r\n"
			<< newbody.str();

	return response.str();
}
