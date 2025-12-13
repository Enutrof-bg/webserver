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
	return "test";
}
std::string handleGET(const std::string &path, const ServerConfig &server)
{
	(void)server;
	std::cout << path << std::endl;
	// a changer
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file)
		throw std::runtime_error("Error: html file open failed");
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