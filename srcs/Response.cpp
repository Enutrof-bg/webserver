#include "../includes/Response.hpp"
#include <sys/wait.h>

Response parseRequest(const std::string &request)
{
	Response rep;
	// std::istringstream stream(request);
	// std::string line;
// std::cout << "Premiers octets (hex): ";
// for (size_t i = 0; i <request.length(); i++)
// {
// 	printf("%02X ", (unsigned char)request[i]);
// 	// printf(":%zu | ", i);
// }
std::cout << std::endl;

	std::cout << "-------------------PARSE REQUEST------------------------" << std::endl;
	size_t header_end = request.find("\r\n\r\n");
	if (header_end == std::string::npos)
	{
		std::cerr << "Erreur: header imcomplet" << std::endl;
		return rep;
	}
	std::string part_header = request.substr(0, header_end);
	rep.body = request.substr(header_end + 4);
	

	std::istringstream stream(part_header);
	std::string line;
	std::getline(stream, line);
	if (!line.empty() && line[line.length() - 1] == '\r')
		line.erase(line.length() - 1);
	// std::cout << "[" << line << "]" << std::endl;
	// std::cout << "TESTCACA" << std::endl;

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
			// if (!value.empty() && value[0] == ' ')
				// value = value.substr(1);
			rep.header[key] = value;
		}
	}
	std::cout <<"----HEADER------" << std::endl;
	std::map<std::string, std::string>::iterator it;
	for (it = rep.header.begin(); it != rep.header.end(); it++)
	{
		std::cout << "First: " <<it->first << " | "<<"Second: "<<it->second << std::endl;
	}
	std::cout << "----BODY------" << std::endl;
	// while (std::getline(stream ,line))
	// {
	// 	std::cout << "TESTBODY line:" <<line << std::endl;
	// 	rep.body.append(line);
	// }
	// std::string remaining((std::istreambuf_iterator<char>(stream)),
	//						std::istreambuf_iterator<char>());
	// rep.body = remaining;
	
	// std::cout << "-------------rep.body:\n" <<rep.body << std::endl;
	std::cout << "-------------rep.body.length():\n" <<rep.body.length() << std::endl;
	std::cout << "-------------------PARSE-REQUEST-FIN---------------------" << std::endl;
	return rep;
}

Location getLocation(const std::string &url, const ServerConfig &server)
{
	if (url.empty())
		std::cout << "ERREUR" << std::endl;
	std::cout << url << std::endl;
	std::string temp_url(url);
	// if (temp_url.size() > 1)
		// temp_url = rtrim(temp_url, "/");
	std::cout << "temp_url:" << temp_url << std::endl;

	// if (temp_url.size() > 1)
	// {
		for (size_t j = 0; j < server._config_location.size(); ++j)
		{
			std::cout << "Location config path:" << server._config_location[j]._config_path << std::endl;
			if (server._config_location[j]._config_path.find(temp_url) != std::string::npos)
			{
				std::cout << "Location returned" << std::endl;
				return (server._config_location[j]);
			}
			std::string temp_path = server._config_location[j]._config_path;
			if (temp_path.length() < url.length()
				&& temp_path.find(".") != std::string::npos)
			{
				if (url.compare(url.length() - temp_path.length(), temp_path.length(), temp_path) == 0)
					return (server._config_location[j]);
			}
		}
	// }

	//return default location / sinon vide
	for (size_t j = 0; j < server._config_location.size(); ++j)
	{
		if (server._config_location[j]._config_path == "/")
		{
			return (server._config_location[j]);
		}
	}
	Location empty_loc;
	return (empty_loc);
}

bool is_directory(const std::string &path) {
    DIR *dir = opendir(path.c_str());
    if (dir != NULL) {
        closedir(dir);
		return true;
    }
    return false;
}

std::string getPath(const std::string &url, const ServerConfig &server, Location &location)
{
	// return "asd";
	std::cout << "-------------------GET PATH------------------------" << std::endl;
	if (url.empty())
		std::cout << "ERREUR (impossible suppose)" << std::endl;
	std::cout << url << std::endl;
	// std::string temp_url(url);
	// temp_url = rtrim(temp_url, "/");
	
	std::cout << "loc:" << location._config_path << std::endl;

	std::string path_root = server._config_root;
	if (!location._config_root.empty())
		path_root = location._config_root;

	std::string path_index = server._config_index;
	if (!location._config_index.empty())
		path_index = location._config_index;
	
	std::string path;
	if (url == "/" || url[url.length() - 1] == '/')
	{
		path = path_root + url;
		//if est un fichier
		if (is_directory(path) == false)
		{
			//erreur 404
			std::cout << "Path is not a directory, 404 error" << std::endl;
			return "";
		}
		else
		{
			std::cout << "path 0: "<< path << std::endl;
			path = path + path_index;
			std::cout << "path 1: "<< path << std::endl;
			return path;
		}
	}
	else
	{
		path = path_root + url;
		if (is_directory(path) == false)
		{
			return path;
		}
		else
		{
			//erreur 301
			std::cout << "Path is a directory, 301 error" << std::endl;
			return "";
		}
	}

	// location._config_root = rtrim(location._config_root, " \t;");
	// if (temp_url.size() > 1)
	// {
	// 	for (size_t j = 0; j < server._config_location.size(); ++j)
	// 	{
	// 		if (server._config_location[j]._config_path.find(temp_url) != std::string::npos)
	// 		{
	// 			std::cout << "loc_test_if:" << server._config_location[j]._config_path << std::endl;
	// 			std::string path = server._config_location[j]._config_root;

	// 			// if (url == "/" || url[url.length() - 1] == '/')
	// 			// {
	// 			// 	path = path + url + server._config_location[j]._config_index;
	// 			// 	//si url est un dossier
	// 			// 	// path = path + "/" + url;

	// 			// 	std::cout << path << std::endl;
	// 			// 	std::cout << "LOCATION2" << std::endl;
	// 			// }
	// 			// else
	// 			// {
	// 			// 	path = path + "/" +server._config_location[j]._config_index;
	// 			// 	std::cout << path << std::endl;
	// 			// 	std::cout << "LOCATION1" << std::endl;
	// 			// }
	// 			std::cout << "LOCATION" << std::endl;
	// 			std::cout << "path: "<< path << std::endl;
	// 			std::cout << "-------------------GET PATH FIN------------------------" << std::endl;
	// 			return path;
	// 		}
	// 	}
	// }

	// std::string path = server._config_root;

	// std::cout << path << std::endl;

	// if (url == "/" || url[url.length() - 1] == '/')
	// {
	// 	path = path + url + server._config_index;
	// }
	// else
	// {
	// 	path = path + url;
	// }
	std::cout << "path 2: "<< path << std::endl;
	std::cout << "-------------------GET PATH FIN 2------------------------" << std::endl;
	return path;
}

int ft_check_method(const Location &loc, const Response &rep)
{
	std::cout << "TEST" << std::endl;
	std::vector<std::string>::const_iterator it = loc._config_allowed_methods.begin();
	for (; it != loc._config_allowed_methods.end(); it++)
	{
		std::cout << "config method:" << *it << std::endl;
		if (rep.method == *it)
			return (0);
	}

	return (1);
}

ParseURL parseURL(const std::string &url, const Response &rep, const ServerConfig &server)
{
	(void)rep;
	(void)server;
	ParseURL result;
	
	size_t query_pos = url.find('?');
	if (query_pos != std::string::npos)
	{
		result.url = url.substr(0, query_pos);
		result.query_string = url.substr(query_pos + 1);
	}
	else
	{
		result.url = url;
		result.query_string = "";  // Vide si pas de ? dans l'URL
	}
	
	// parcour les location de server pour trouver si url correspond a un cgi
	// for (size_t j = 0; j < server._config_location.size(); ++j)
	// {
	// 	std::string cgi_path = server._config_location[j]._config_cgi_path;
	// 	std::vector<std::string> cgi_exts = server._config_location[j]._config_cgi_ext;
	// 	if (!cgi_path.empty())
	// 	{
	// 		//verifie si l'url se termine par une des extensions cgi
	// 		for (size_t i = 0; i < cgi_exts.size(); ++i)
	// 		{
	// 			size_t ext_pos = result.url.rfind(cgi_exts[i]);
	// 			if (ext_pos != std::string::npos
	// 				&& ext_pos + cgi_exts[i].length() == result.url.length())
	// 			{
	// 				result.path_script = result.url;
	// 				result.path_info = "";
	// 				return result;
	// 			}
	// 		}
	// 	}
	// }

	//check tout les location pour trouver les extensions CGI
	for (size_t j = 0; j < server._config_location.size(); ++j)
	{
		std::vector<std::string> cgi_exts = server._config_location[j]._config_cgi_ext;
		if (cgi_exts.empty())
			continue;
		
		//parcours des extensions CGI pour cette location
		for (size_t i = 0; i < cgi_exts.size(); ++i)
		{
			std::string ext = cgi_exts[i];
			size_t ext_pos = result.url.find(ext);
			
			if (ext_pos != std::string::npos)
			{
				size_t script_end = ext_pos + ext.length();
				result.path_script = result.url.substr(0, script_end);
				

				if (script_end < result.url.length())
					result.path_info = result.url.substr(script_end);
				else
					result.path_info = "";
				
				return result;
			}
		}
	}

	// aucun cgi trouve
	result.path_script = result.url;
	result.path_info = "";
	
	return result;
}

std::string ft_check_body_size(const Response &rep, const ServerConfig &server, const Location &loc)
{
	(void)rep;
	(void)server;
	(void)loc;
	size_t max_size = server._config_client_max_body_size;
	if (loc._config_client_max_body_size > 0)
		max_size = loc._config_client_max_body_size;
	std::cout << "Max body size allowed: " << max_size << " bytes" << std::endl;

	std::map<std::string, std::string>::const_iterator it = rep.header.find("Content-Length");
	if (it != rep.header.end())
	{
		size_t content_length = atoi(it->second.c_str());
		std::cout << "content_length_test:"<< content_length << std::endl;
		if (content_length > max_size)
		{
			std::cout << "Body size exceeded" << std::endl;
			std::cout << "content_length:" << content_length << std::endl;
			return "HTTP/1.1 413 Payload Too Large\r\n\r\n<h1>ERROR 413 Payload Too Large</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
		}
	}

	if (rep.body.length() > max_size)
	{
		std::cout << "Body size exceeded" << std::endl;
		std::cout << "rep.body.length():" << rep.body.length() << std::endl;
		return "HTTP/1.1 413 Payload Too Large\r\n\r\n<h1>ERROR 413 Payload Too Large</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
	}

	return "";
}

std::string ft_redirection(const ServerConfig &server, ParseURL &parsed_url)
{
	for (size_t j = 0; j < server._config_location.size(); ++j)
	{
		Location loc = server._config_location[j];
		if (!loc._config_redirect.empty())
		{
			if (parsed_url.url == loc._config_path)
			{
				std::string new_url = loc._config_redirect;
				if (parsed_url.query_string != "")
					new_url += "?" + parsed_url.query_string;

				std::string body = "<h1>301 Moved Permanently</h1><p>The document has moved <a href=\"" + new_url + "\">here</a>.</p>";
				std::ostringstream response;
				response << "HTTP/1.1 301 Moved Permanently\r\n"
						 << "Location: " << new_url << "\r\n"
						 << "Content-Type: text/html; charset=UTF-8\r\n"
						 << "Content-Length: " << body.length() << "\r\n"
						 << "Connection: close\r\n"
						 << "\r\n"
						 << body;
				
				return response.str();
			}
			else
			{
				//verifie si parsed_url.path_script commence par loc._config_path
				std::string loc_path = loc._config_path;
				if (!loc_path.empty() && loc_path[0] == '/')
					loc_path = loc_path.substr(1);
				//segmente parsed_url.path_script et verifie si correspond a loc_path
				std::string temp_right = parsed_url.path_script;
				std::string temp_left;
				size_t pos = temp_right.find('/');
				while (pos != std::string::npos)
				{
					temp_left = temp_right.substr(0, pos);
					temp_right = temp_right.substr(pos + 1);
					std::cout << "temp_left: " << temp_left << std::endl;
					std::cout << "temp_right:" << temp_right << std::endl;
					if (temp_left == loc_path)
					{
						std::string new_url = loc._config_redirect + "/" + temp_right;
						std::cout << "New URL for redirection: " << new_url << std::endl;
						if (parsed_url.query_string != "")
							new_url += "?" + parsed_url.query_string;

						std::string body = "<h1>301 Moved Permanently</h1><p>The document has moved <a href=\"" + new_url + "\">here</a>.</p>";
						std::ostringstream response;
						response << "HTTP/1.1 301 Moved Permanently\r\n"
								<< "Location: " << new_url << "\r\n"
								<< "Content-Type: text/html; charset=UTF-8\r\n"
								<< "Content-Length: " << body.length() << "\r\n"
								<< "Connection: close\r\n"
								<< "\r\n"
								<< body;
						
						return response.str();
					}
					pos = temp_right.find('/');
				}
				//si ca correspond, fait la redirection
			
			}
			
		}
	}
	return "";
}

std::string getRequest(const Response &rep, const ServerConfig &server, Server &srv, ClientState &client_state)
{
	//error a gerer check method
	// (void)server;
	// return "caca1";
	// std::cout << rep.url << std::endl;
	(void)client_state;
	std::cout << "-------------------GET REQUEST------------------------" << std::endl;

	ParseURL parsed_url = parseURL(rep.url, rep, server);
	std::cout << "Parsed URL:" << std::endl;
	std::cout << "  Full URL: " <<  "{"<< parsed_url.url << "}" << std::endl;
	std::cout << "  Script Path: " <<  "{"<< parsed_url.path_script << "}" << std::endl;
	std::cout << "  Path Info: " <<  "{"<< parsed_url.path_info << "}" << std::endl;
	std::cout << "  Query String: " << "{"<< parsed_url.query_string << "}" << std::endl;

	//verification des redirections
	std::string test_redir = ft_redirection(server, parsed_url);
	if (!test_redir.empty())
		return test_redir;

	Location loc = getLocation(parsed_url.path_script, server);
	std::cout << "loc path:"<<loc._config_path << std::endl;
	std::cout << "method size:"<<loc._config_allowed_methods.size() << std::endl;
	std::vector<std::string>::const_iterator it = loc._config_allowed_methods.begin();
	for (; it != loc._config_allowed_methods.end(); it++)
	{
			std::cout << "config method:" << *it << std::endl;
	}

	//gestion transfer-encoding: chunked
	if (rep.header.find("Transfer-Encoding") != rep.header.end()
		&& rep.header.at("Transfer-Encoding") == "chunked")
	{
		// if (rep.method != "POST" && rep.method != "PUT")
		// {
		// 	std::cout << "Chunked encoding only allowed for POST and PUT methods" << std::endl;
		// 	return "HTTP/1.1 400 Bad Request\r\n\r\n<h1>ERROR 400 Bad Request</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
		// }
		std::string body;
		size_t pos = 0;
		while (true)
		{
			size_t line_end = rep.body.find("\r\n", pos);
			if (line_end == std::string::npos)
				break;

			std::string size_str = rep.body.substr(pos, line_end - pos);
			size_t chunk_size = std::strtoul(size_str.c_str(), NULL, 16);
			if (chunk_size == 0)
				break;

			pos = line_end + 2;
			body.append(rep.body.substr(pos, chunk_size));
			pos += chunk_size + 2; // +2 pour \r\n

			//verifie que le chunk depasse pas la taille max autorisee
			//si location existe, check sa taille max
			//sinon check server
			size_t current_body_size = body.length();
			size_t max_size = server._config_client_max_body_size;
			if (loc._config_client_max_body_size > 0)
				max_size = loc._config_client_max_body_size;
			if (current_body_size > max_size)
			{
				std::cout << "Body size exceeded during chunked decoding" << std::endl;
				return "HTTP/1.1 413 Payload Too Large\r\n\r\n<h1>ERROR 413 Payload Too Large</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
			}
		}
		const_cast<Response&>(rep).body = body;
		
		//met a jour le content-length ou le creer si existe pas
		// const_cast<Response&>(rep).header["Content-Length"] = std::to_string(body.length());

	}
	std::cout << rep.body << std::endl;
	std::cout << "Reconstructed body length:" << rep.body.length() << std::endl;
	
	//check body size
	std::string body_size_check = ft_check_body_size(rep, server, loc);
	if (!body_size_check.empty())
		return body_size_check;


	std::cout << "TEST1" << std::endl;

	if (!loc._config_allowed_methods.empty() && ft_check_method(loc, rep) == 1)
	{
		std::cout << "TEST5" << std::endl;
		return "HTTP/1.1 405 Method Not Allowed\r\n\r\n<h1>ERROR 405 Method Not Allowed</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
	}
	std::cout << "TEST6" << std::endl;
	std::string path = getPath(rep.url, server, loc);
	if (path.empty())
	{
		//verifie si c'est une redirection 301
		std::string temp_path = server._config_root;
		if (!loc._config_root.empty())
			temp_path = loc._config_root;

		if (rep.url[rep.url.length() - 1] != '/')
		{
			std::string new_url = rep.url + "/";
			std::string body = "<h1>301 Moved Permanently</h1><p>The document has moved <a href=\"" + new_url + "\">here</a>.</p>";
			std::ostringstream response;
			response << "HTTP/1.1 301 Moved Permanently\r\n"
					 << "Location: " << new_url << "\r\n"
					 << "Content-Type: text/html; charset=UTF-8\r\n"
					 << "Content-Length: " << body.length() << "\r\n"
					 << "Connection: close\r\n"
					 << "\r\n"
					 << body;
			
			return response.str();
		}

		return "HTTP/1.1 404 Not Found\r\n\r\n<h1>ERROR 404 Not Found</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
	}

	
	// return "caca";
	// if (rep.url.find("cgi-bin") != std::string::npos
	// 	|| rep.url.find(".py") != std::string::npos
	// 	|| rep.url.find(".php") != std::string::npos)

	std::string temp_path = loc._config_path;
	// std::string temp_path = parsed_url.path_script;
	std::cout << "temp_path:" << temp_path << std::endl;

	if (temp_path.length() > 1
		&& parsed_url.path_script.compare(parsed_url.path_script.length() - temp_path.length(), temp_path.length(), temp_path) == 0
		&& parsed_url.path_script.find(".") != std::string::npos)
	{
		std::cout << "going to handleCGI" << std::endl;
		return handleCGI(rep, server, path, loc, parsed_url, srv, client_state);
	}
	if (rep.method == "GET")
	{
		return handleGET(path, server, loc, parsed_url);
	}
	else if (rep.method == "POST")
	{
		return handlePOST(rep, server);
	}
	else if (rep.method == "DELETE")
	{
		return handleDELETE(rep, server, loc);
	}
	// return "no method found";
	return ("HTTP/1.1 405 Method Not ALlowed\r\n\r\n<h1>ERROR 405 Method Not Allowed</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>");
}

std::string ft_get_extension_file(const std::string& path)
{
	size_t dot_pos = path.find_last_of('.');
	if (dot_pos == std::string::npos) return "text/plain";

	std::string ext = path.substr(dot_pos);
	if (ext == ".html")
		return "text/html";
	if (ext == ".css")
		return "text/css";
	if (ext == ".js")
		return "application/javascript";
	if (ext == ".png")
		return "image/png";
	if (ext == ".jpg" || ext == ".jpeg")
		return "image/jpeg";
	return "text/plain";
}

std::string handleGET(const std::string &path, const ServerConfig &server, const Location &loc, const ParseURL &parsed_url)
{
	(void)server;
	(void)loc;
	(void)parsed_url;
	std::cout << "-----------------------------------HANDLE_GET----------------" <<std::endl;
	std::cout << "Chemin apth: " << path << std::endl;
	std::cout << "parsed url: " << parsed_url.url << std::endl;
	
	std::string temp_content_type = ft_get_extension_file(path);

	DIR *dir = opendir(path.c_str());
	if (dir != NULL)
	{
		if (loc._config_autoindex == true)
		{
			std::cout << "Autoindex activé pour le répertoire: " << path << std::endl;
			std::ostringstream body;
			body << "<!DOCTYPE html>\n"
				 << "<html>\n<head><title>Index of " << parsed_url.url << "</title></head>\n"
				 << "<body>\n"
				 << "<h1>Index of " << parsed_url.url << "</h1>\n"
				 << "<ul>\n";

			struct dirent *ent;
			while ((ent = readdir(dir)) != NULL)
			{
				std::string filename = ent->d_name;
				if (filename == ".")
					continue;
				body << "<li><a href=\"" << parsed_url.url;
				if (parsed_url.url[parsed_url.url.length() - 1] != '/')
					body << "/";
				body << filename << "\">" << filename << "</a></li>\n";
			}
			closedir(dir);

			body << "</ul>\n</body>\n</html>\n";

			std::ostringstream response;
			response << "HTTP/1.1 200 OK\r\n"
					 << "Content-Type: " << temp_content_type << "; charset=UTF-8\r\n"
					 << "Content-Length: " << body.str().length() << "\r\n"
					 << "Connection: close\r\n"
					 << "\r\n"
					 << body.str();
			
			return response.str();
		}
		else
		{
			closedir(dir);
			// Autoindex desactive
			std::string body = "<h1>403 Forbidden</h1><p>Directory listing not allowed</p>";
			std::ostringstream response;
			response << "HTTP/1.1 403 Forbidden\r\n"
					 << "Content-Type: " << temp_content_type << "; charset=UTF-8\r\n"
					 << "Content-Length: " << body.length() << "\r\n"
					 << "Connection: close\r\n"
					 << "\r\n"
					 << body;
			return response.str();
		}
	}
	
	std::string line;
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		if (loc._config_autoindex == true)
		{
			std::string dir_path;
			if (!loc._config_root.empty())
				dir_path = loc._config_root;
			else
				dir_path = server._config_root;
			
			if (parsed_url.url != "/" && !parsed_url.url.empty())
			{
				if (dir_path[dir_path.length() - 1] != '/' && parsed_url.url[0] != '/')
					dir_path += "/";
				dir_path += parsed_url.url;
			}

			DIR *dir2 = opendir(dir_path.c_str());
			if (dir2 != NULL)
			{
				std::ostringstream body;
				body << "<!DOCTYPE html>\n"
					 << "<html>\n<head><title>Index of " << parsed_url.url << "</title></head>\n"
					 << "<body>\n"
					 << "<h1>Index of " << parsed_url.url << "</h1>\n"
					 << "<ul>\n";

				struct dirent *ent;
				while ((ent = readdir(dir2)) != NULL)
				{
					std::string filename = ent->d_name;
					if (filename == ".")
						continue;
					body << "<li><a href=\"" << parsed_url.url;
					if (!parsed_url.url.empty() && parsed_url.url[parsed_url.url.length() - 1] != '/')
						body << "/";
					body << filename << "\">" << filename << "</a></li>\n";
				}
				closedir(dir2);

				body << "</ul>\n</body>\n</html>\n";

				
				std::ostringstream response;
				response << "HTTP/1.1 200 OK\r\n"
						 << "Content-Type: " << temp_content_type << "; charset=UTF-8\r\n"
						 << "Content-Length: " << body.str().length() << "\r\n"
						 << "Connection: close\r\n"
						 << "\r\n"
						 << body.str();
				
				std::cout << "-----------------------------------HANDLE_GET-FIN------------" << std::endl;
				return response.str();
			}
		}
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
				// while (getline(error_file, line))
				// {
				// 	body << line;
				// }
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
				 << "Content-Type: " << temp_content_type << "; charset=UTF-8\r\n"
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
			<<"Content-Type: " << temp_content_type << "; charset=UTF-8\r\n"
			<<"Content-Length: " << content.length()<<"\r\n"
			<<"Connection: close\r\n"
			<<"\r\n"
			<< content;
	std::cout << "-----------------------------------HANDLE_GET-FIN-------------" <<std::endl;
	return (response.str());
}

std::string handlePOST(const Response &rep, const ServerConfig &server)
{
	(void)rep;
	(void)server;
	std::cout << "-----------------------------------HANDLE_POST_BODY----------------" <<std::endl;
	std::cout << "rep.body:" << rep.body  << std::endl;
	std::string post_content_type;

	//verifie transfer-encoding dans le header vaut chunked
	if (rep.header.find("Transfer-Encoding") != rep.header.end()
		/*&& rep.header.at("Transfer-Encoding") == "chunked"*/)
	{
		std::cout << "POST with chunked transfer-encoding not supported yet" << std::endl;
		return "HTTP/1.1 501 Not Implemented\r\n\r\n<h1>ERROR 501 Not Implemented</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
	}

	//check rep.length or error413
	std::map<std::string, std::string>::const_iterator it_len = rep.header.find("Content-Length");
	if (it_len != rep.header.end())
	{
		size_t content_length = atoi(it_len->second.c_str());
		std::cout << "content_length_test:"<< content_length << std::endl;
	}

	std::map<std::string, std::string>::const_iterator it_type = rep.header.find("Content-Type");
	if (it_type != rep.header.end())
	{
		std::cout << "content-type_test: " <<it_type->second << std::endl;
		post_content_type = it_type->second;
	}
	// for (it = rep.header.begin(); it != rep.header.end(); it++)
	// {

	// }
	//parsing body
	// std::map<std::string, std::string>::const_iterator it_type;
	if (post_content_type == "application/x-www-form-urlencoded")
	{
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
			<< "<html>\n<head><title>POST recu</title></head>\n"
			<< "<body>\n"
			<< "<h1>Donnees recue</h1>\n"
			<< "<ul>\n";

		for (std::map<std::string, std::string>::iterator it = data.begin();
			it != data.end(); it++)
			{
				newbody << "<li><b>" << it->first << "</b>:" << it->second << "</li>\n";
			}
			newbody << "</ul>\n"
			<< "<p>Body brut: <code>" << rep.body << "</code></p>\n"
			<< "<br><p><a title=\"Motherfucking Website\" href=\"/\">go back</a></p></br>\n"
			<< "</body>\n</html>\n";

		std::ostringstream response;
		response << "HTTP/1.1 201 Created\r\n"
				<< "Content-Type: text/html; charset=UTF-8\r\n"
				<< "Content-Length: " << newbody.str().length() << "\r\n"
				<< "Connection: close\r\n"
				<< "\r\n"
				<< newbody.str();
		std::cout << "-----------------------------------HANDLE_POST_FIN----------------" <<std::endl;
		return response.str();
	}
	std::cout << "TEST:"<<post_content_type << std::endl;
	// size_t it_content_type = post_content_type.find("multipart/form-data");
	// if (it_content_type != post_content_type.end())
	// {

	// }
	if (post_content_type.find("multipart/form-data;") != std::string::npos)
	{
		std::cout << "TEST:"<<post_content_type << std::endl;
		//extraire boundary
		size_t pos_boundary = post_content_type.find("boundary=");
		std::string boundary = post_content_type.substr(pos_boundary + 9);
		std::string delimiter = "--" + boundary;
		std::cout << "==========BOUNDARY_TEST:==============" << std::endl;
		std::cout << boundary << std::endl;
		std::cout << delimiter << std::endl;
		std::cout << "==========BOUNDARY_TEST_end:==========\n\n"  << std::endl;
		
		//parser body grace au boundary
		size_t pos = 0;
		while ((pos = rep.body.find(delimiter, pos)) != std::string::npos)
		{
			pos += delimiter.length();
			size_t end_pos = rep.body.find(delimiter, pos);
			// if (end_pos == std::string::npos)
				// break;
			
			std::string newbody = rep.body.substr(pos, end_pos - pos);
			std::cout << "=====TEST_PRINT_BOUNDARY_BODY========" << std::endl;
			std::cout << newbody << std::endl;
			std::cout << "=====TEST_PRINT_BOUNDARY_BODY_END====\n\n" << std::endl;

			//separer header et contenu
			std::cout << "=====TEST_PRINT_separete_BODY========" << std::endl;
			size_t pos_header = newbody.find("\r\n\r\n");
			if (pos_header == std::string::npos)
			{	
				std::cout << "continue... (/r/n/r/n not found)" << std::endl;
				continue;
			}

			std::string part_header = newbody.substr(0, pos_header);
			std::string part_body = newbody.substr(pos_header + 4);

			std::cout << "-------------part_header_boundary----------" <<std::endl;
			std::cout << part_header << std::endl;
			std::cout << "-------------part_body_boundary----------" <<std::endl;
			std::cout << part_body << std::endl;
			std::cout << "part_body_length:"<< part_body.length() << std::endl;

			std::cout << "Taille du fichier: " << part_body.length() << " octets" << std::endl;
			std::cout << "Premiers octets (hex): ";
			for (size_t i = 0; i < 20 && i < part_body.length(); i++)
			{
				printf("%02X ", (unsigned char)part_body[i]);
			}
			std::cout << std::endl;


			std::cout << "=====TEST_PRINT_separete_BODY_end========" << std::endl;

			if (part_header.find("filename=") != std::string::npos)
			{
				size_t filename_pos = part_header.find("filename=\"");
				size_t filename_pos_end = part_header.find("\"", filename_pos + 10);
				std::string new_filename = part_header.substr(filename_pos + 10, filename_pos_end - (filename_pos + 10));
				std::cout << "test_test_new_filename:"<<  new_filename << std::endl;

				if (part_body.length() >=2
					&& part_body[part_body.length() - 1] == '\n'
					&& part_body[part_body.length() - 2] == '\r')
				{
					part_body = part_body.substr(0, part_body.length() - 2);
				}
				std::cout << "-------------part_body_boundary----------" <<std::endl; 
				std::cout << part_body << std::endl;
				std::string upload_path = "./uploads/" + new_filename;
				std::cout << "test_test_new_filename:"<<  upload_path << std::endl;

				std::ofstream output(upload_path.c_str(), std::ios::binary);
				if (output.is_open())
				{
					output.write(part_body.c_str(), part_body.length());
					output.close();
				}
			}

			//extraire filename si present
			//

		}

		//extraire filename avec COntenDispoitiotn
		//lire le sdonne binaire
		//ecrire dans un fichier dans /upload ?
		//retourner 201 Created avec le chemin du fichier?
		std::ostringstream body;
		body << "<body><h1>Upload réussi!</h1>"
			 << "<p><a title=\"Retour\" href=\"/\">go back</a></p></body>";

		std::ostringstream response;
		response << "HTTP/1.1 201 Created\r\n"
			 << "Content-Type: text/html; charset=UTF-8\r\n"
			 << "Content-Length: " << body.str().length() <<"\r\n"
			 << "\r\n"
			 << body.str();
		return (response.str());
	}
	// else if (post_content_type == "multipart/form-data")
	// else if (post_content_type.find("multipart/form-data"))
	// {
	// 	return ("CACA");
	// }
	// else
	// {
		return "PROUST";
	// }
}

std::string handleDELETE(const Response &rep, const ServerConfig &server, Location &loc)
{
	(void)rep;
	(void)server;
	//path
	std::cout << "-----------------------------------HANDLE_DELETE-------------" <<std::endl;
	std::cout << "URL1:" <<rep.url.c_str() << std::endl;
	std::string path = getPath(rep.url.c_str(), server, loc);
	std::cout << "URL2:" << path << std::endl;
	// path = "../" + path;
	// std::cout << "URL3:" << path << std::endl;
	std::ostringstream newbody;
	std::ostringstream response;
	//check path if exist
	// std::ifstream test(path.c_str());
	// if (!test.good())
	// {
	// 	std::cout << "good" << std::endl;
	// }
	// else
	// {
	// 	std::cout << "badasdasd" << std::endl;
	// }

	// test.close();
	// close(test);

	//path end with '/'
	if (path[path.length() -1] == '/')
	{
		newbody << "<!DOCTYPE html>\n"
			<< "<html>\n<head><title>DELETE reçu</title></head>\n"
			<< "<body>\n"
			<< "<h1>Deletion didnt work "<< rep.url <<" </h1>\n"
			<< "<ul>\n";
		response << "HTTP/1.1 400 Bad Request\r\n"
			<< "Content-Type: text/html; charset=UTF-8\r\n"
			<< "Content-Length: " << newbody.str().length() << "\r\n"
			<< "Connection: close\r\n"
			<< "\r\n"
			<< newbody.str();
		std::cout << "-----------------------------------HANDLE_DELETE-FIN---------" <<std::endl;
		return response.str();
	}

	//delete file
	if (std::remove(path.c_str()))
	{
		newbody << "<!DOCTYPE html>\n"
			<< "<html>\n<head><title>DELETE reçu</title></head>\n"
			<< "<body>\n"
			<< "<h1>Deletion didnt work "<< rep.url <<" </h1>\n"
			<< "<ul>\n";
		response << "HTTP/1.1 404 Not Found\r\n"
			<< "Content-Type: text/html; charset=UTF-8\r\n"
			<< "Content-Length: " << newbody.str().length() << "\r\n"
			<< "Connection: close\r\n"
			<< "\r\n"
			<< newbody.str();
		std::cout << "-----------------------------------HANDLE_DELETE-FIN---------" <<std::endl;
		return response.str();
	}
	else
	{
		newbody << "<!DOCTYPE html>\n"
			<< "<html>\n<head><title>DELETE reçu</title></head>\n"
			<< "<body>\n"
			<< "<h1>deleted "<< rep.url <<" </h1>\n"
			<< "<ul>\n";
	}


	response << "HTTP/1.1 204 No Content\r\n"
			<< "Content-Type: text/html; charset=UTF-8\r\n"
			<< "Content-Length: " << newbody.str().length() << "\r\n"
			<< "Connection: close\r\n"
			<< "\r\n"
			<< newbody.str();
	std::cout << "-----------------------------------HANDLE_DELETE-FIN---------" <<std::endl;
	return response.str();

	// return "DELETEtest";
}

void ft_free_double_tab(char **tab)
{
	int		i;

	i = 0;
	if (!tab)
		return ;
	while (tab[i])
	{
		delete[] tab[i];
		i++;
	}
	delete[] tab;
}

char *ft_strdup(const char *s1)
{
	char	*dup;
	size_t	i;

	dup = new char[strlen(s1) + 1];
	if (!dup)
		return (NULL);
	i = 0;
	while (s1[i])
	{
		dup[i] = s1[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

char	**ft_create_add_new_tab(char *str, char **tab, int size)
{
	int		i;
	char	**newtab;

	i = 0;
	newtab = new char*[size + 2];
	if (!newtab)
		return (NULL);
	while (tab[i])
	{
		newtab[i] = ft_strdup(tab[i]);
		if (!newtab[i])
		{
			ft_free_double_tab(newtab);
			return NULL;
		}
		i++;
	}
	newtab[i] = ft_strdup(str);
	if (!newtab[i])
	{
		ft_free_double_tab(newtab);
		return NULL;
	}
	i++;
	newtab[i] = 0;
	return (newtab);
}
//from minishell :)
char	**ft_add_double_tab(char *str, char **tab)
{
	int		i;
	char	**newtab;

	i = 0;
	if (!str)
		return (tab);
	if (tab == NULL)
	{
		newtab = new char*[2];
		if (!newtab)
			return (NULL);
		newtab[0] = ft_strdup(str);
		if (!newtab[0])
		{
			delete[] newtab;
			return NULL;
		}
		return (newtab[1] = 0, newtab);
	}
	while (tab[i])
		i++;
	newtab = ft_create_add_new_tab(str, tab, i);
	return (ft_free_double_tab(tab), newtab);
}

void ft_print_double_tab(char **tab)
{
	int		i;

	i = 0;
	if (!tab)
	{
		std::cout << "tab is NULL" << std::endl;
		return ;
	}
	while (tab[i])
	{
		std::cout << "tab[" << i << "]:" << "{" << tab[i] << "}" << std::endl;
		i++;
	}
}


std::string intToString(size_t n)
{
	std::ostringstream oss;
	oss << n;
	return oss.str();
}

char **ft_return_cgi_env(const Response &rep, const ServerConfig &server,
						std::string path, const Location &loc, const ParseURL &parsed_url)
{
	(void)rep;
	(void)server;
	(void)path;
	(void)loc;
	char **newenv;
	newenv = NULL;
	newenv = ft_add_double_tab(const_cast<char*>(("REQUEST_METHOD=" + rep.method).c_str()), newenv);
	newenv = ft_add_double_tab(const_cast<char*>(("QUERY_STRING=" + parsed_url.query_string).c_str()), newenv);

	//si content-length dans header n'existe pas, calculer a partir de body
	if ((rep.header.find("Content-Length")) != rep.header.end())
		newenv = ft_add_double_tab(const_cast<char*>(("CONTENT_LENGTH=" + rep.header.find("Content-Length")->second).c_str()), newenv);
	else
		newenv = ft_add_double_tab(const_cast<char*>(("CONTENT_LENGTH=" + intToString(rep.body.length())).c_str()), newenv);

	if ((rep.header.find("Content-Type")) != rep.header.end())
		newenv = ft_add_double_tab(const_cast<char*>(("CONTENT_TYPE=" + rep.header.find("Content-Type")->second).c_str()), newenv);
	// newenv = ft_add_double_tab(const_cast<char*>(("PATH_INFO=" + path).c_str()), newenv);
	// newenv = ft_add_double_tab(const_cast<char*>(("PATH_TRANSLATED=" + path).c_str()), newenv);
	newenv = ft_add_double_tab(const_cast<char*>(("SCRIPT_NAME=" + parsed_url.path_script).c_str()), newenv);
	newenv = ft_add_double_tab(const_cast<char*>(("SERVER_NAME=" + server._config_server_name).c_str()), newenv);
	newenv = ft_add_double_tab(const_cast<char*>(("SERVER_PROTOCOL=" + rep.version).c_str()), newenv);
	// newenv = ft_add_double_tab(const_cast<char*>(("SERVER_PROTOCOL=" + rep.version).c_str()), newenv);
	newenv = ft_add_double_tab(const_cast<char*>(("PATH_INFO=" + parsed_url.path_info).c_str()), newenv);
	return (newenv);
}

std::string handleCGI(const Response &rep, const ServerConfig &server,
						std::string path, const Location &loc,
						const ParseURL &parsed_url, Server &srv,
						ClientState &client_state)
{
	(void)rep;
	(void)server;
	(void)path;
	(void)loc;
	(void)parsed_url;
	(void)srv;
	(void)client_state;
	std::cout << "=======================CGI HANDLE================" << std::endl;
	std::cout << "method:"  << rep.method << std::endl;
	std::cout << "url:" << rep.url << std::endl;
	std::cout << "version:"  << rep.version << std::endl;
	std::cout << "body:" << rep.body << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = rep.header.begin(); it != rep.header.end(); it++)
	{
		std::cout << "header->key: " << it->first << " | header->second: " << it->second << std::endl;
	}

	std::cout << "=======================CGI HANDLE FIN================\n" << std::endl;
	// std::string cgi_path;
	// Location cgi_bin;
	// cgi_bin = server.find()
	// std::vector<Location>::const_iterator it = server._config_location.begin();
	// for (; it != server._config_location.end(); it++)
	// {
	// 	if (it->_config_path.find("cgi-bin") != std::string::npos)
	// 	{
	// 		cgi_path = it->_config_cgi_pass;
	// 	}
	// }

	std::cout << "PathCGI1:" << path << std::endl;

	std::string temp_cgi_path = loc._config_cgi_path;
	std::cout << "PathCGI2:" << temp_cgi_path << std::endl;

	//check le path script si existe
	std::ifstream script_path;
	script_path.open(temp_cgi_path.c_str());
	if (!script_path.is_open())
	{
		return "HTTP/1.1 404 Not Found\r\n\r\n<h1>Script not found</h1>";
	}
	script_path.close();

	//pipe
	int scriptfd[2];
	if (pipe(scriptfd) < 0)
	{
		return "HTTP/1.1 500 Internal Server Error\r\n\r\n<h1>ERROR 500 Pipe Error</h1>";
	}

	// Pipe en mode non-bloquant pour utilisation avec poll()
	fcntl(scriptfd[0], F_SETFL, O_NONBLOCK);
	
	//pipe pour body
	int bodyfd[2];
	if (rep.method == "POST")
	{
		if (pipe(bodyfd) < 0)
		{
			close(scriptfd[0]);
			close(scriptfd[1]);
			return "HTTP/1.1 500 Internal Server Error\r\n\r\n<h1>ERROR 500 Pipe Error</h1>";
		}
	}

	//fork
	int id = fork();
	if (id < 0)
	{
		//PROTEC
		return "HTTP/1.1 500 Internal Server Error\r\n\r\n<h1>ERROR 500 Fork Error</h1>";
	}

	//processus enfant
	if (id == 0)
	{
		std::cout << "==============execve_enfant==========" << std::endl;
		//def envp du cgi
		char *argv[] = {const_cast<char*>(path.c_str()), NULL};
		// char *envp[] = {NULL};
		char **envp = ft_return_cgi_env(rep, server, path, loc, parsed_url);
		ft_print_double_tab(envp);

		//redir stdout vers pipe
		close(scriptfd[0]);
		dup2(scriptfd[1], STDOUT_FILENO);
		close(scriptfd[1]);

		//redir stdin post
		if (rep.method == "POST")
		{
			close(bodyfd[1]);
			dup2(bodyfd[0], STDIN_FILENO);
			close(bodyfd[0]);
		}
	

		// dup2(scriptfd[0], 0);
		
		//executer le script
		execve(temp_cgi_path.c_str(), argv, envp);
		//execve fail
		ft_free_double_tab(envp);
		// std::string error_msg = "HTTP/1.1 500 Internal Server Error\r\n\r\n<h1>ERROR 500 Execve Error</h1>";
		// write(1, error_msg.c_str(), error_msg.length());
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "================parent================" << std::endl;
		if (rep.method == "POST")
		{
			//envoyer le body au script
			close(bodyfd[0]);
			write(bodyfd[1], rep.body.c_str(), rep.body.length());
			close(bodyfd[1]);
		}

		client_state.cgi_pid = id;
		client_state.fd_cgi = scriptfd[0];
		client_state.state = ClientState::READING_CGI;
		client_state.last_activity = time(NULL);

		//wait processus enfant
		int status = 0;
		(void)status;
		// int cgi_timeout = 5;
		// int elapsed = 0;
		// pid_t wait_result;
		// while (elapsed < cgi_timeout)
		// {
		// 	wait_result = waitpid(id, &status, WNOHANG);
		// 	if (wait_result == id)
		// 	{
		// 		break; //processus termine
		// 	}
		// 	if (wait_result == -1)
		// 	{
		// 		//erreur waitpid
		// 		close(scriptfd[0]);
		// 		close(scriptfd[1]);
		// 		return "HTTP/1.1 500 Internal Server Error\r\n\r\n<h1>ERROR 500 Waitpid Error</h1>";
		// 	}
		// 	usleep(1000000); // check tout les 1 seconde
		// 	elapsed++;
		// }

		// if (elapsed == cgi_timeout)
		// {
		// // 	//timeout
		// 	kill(id, SIGKILL);
		// 	waitpid(id, &status, 0);//processus zombie
		// 	close(scriptfd[0]);
		// 	close(scriptfd[1]);
		// 	return "HTTP/1.1 504 Gateway Timeout\r\n\r\n<h1>ERROR 504 CGI Timeout</h1><ap><a title=\"GO BACK\" href=\"/\">go back</a></p>";
		// }

		// waitpid(id, &status, 0);
		// close(scriptfd[0]);


		
		close(scriptfd[1]);
		//envoyer le body au script si post
		// std::string buff_output;
		// char buffer[4096];
		// size_t buffer_read;

		// while ((buffer_read = read(scriptfd[0], buffer, sizeof(buffer))) > 0)
		// {

		// 	std::cout << "Read " << buffer_read << " bytes from CGI" << std::endl;
		// 	buff_output.append(buffer, buffer_read);


			
		// }
		std::cout << "================parent==READ_FIN===============" << std::endl;

		// close(scriptfd[0]);

		
		// std::cout << "\n[BUFFER:"<< buffer << "]"<< std::endl;
		std::cout << "================parent==FIN===============" << std::endl;

	
		if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
		{
			return "HTTP/1.1 500 Internal Server Error\r\n\r\n<h1>ERROR 500 Execve Error</h1>";
		}

		std::string response = "";
		// std::ostringstream response;
		// response << "HTTP/1.1 200 OK\r\n"
					//  << buff_output;
		return response;
	}
	return "HTTP/1.1 500 Internal Server Error\r\n\r\n<h1>ERROR 500 Server error</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
	// return ("caca");
}

