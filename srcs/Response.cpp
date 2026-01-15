#include "../includes/Response.hpp"

//Parse la requete http et retoune une structure Response
Response parseRequest(const std::string &request)
{
	Response rep;

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
	std::cout <<"----HEADER------" << std::endl;
	std::map<std::string, std::string>::iterator it;
	for (it = rep.header.begin(); it != rep.header.end(); it++)
	{
		std::cout << "First: " <<it->first << " | "<<"Second: "<<it->second << std::endl;
	}
	std::cout << "----BODY------" << std::endl;

	std::cout << "-------------rep.body.length():\n" <<rep.body.length() << std::endl;
	std::cout << "-------------------PARSE-REQUEST-FIN---------------------" << std::endl;
	return rep;
}

//Cherche la location correspondant a l'url
//Retourne la location correspondante ou la location par defaut / si definie
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
			//verifie que temp_url est un prefix de la location
			if (server._config_location[j]._config_path.length() <= temp_url.length()
				&& temp_url.compare(0, server._config_location[j]._config_path.length(), server._config_location[j]._config_path) == 0
				&& server._config_location[j]._config_path != "/")
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

//Retourne le path absolu du fichier a servir
//Cherche dans la config server et location le root et index
//Si url se termine par / , verifie que c'est un dossier puis ajoute l'index
//Sinon verifie que c'est un fichier
std::string getPath(const std::string &url, const ServerConfig &server, Location &location)
{
	std::cout << "-------------------GET PATH------------------------" << std::endl;
	if (url.empty())
		std::cout << "ERREUR (impossible suppose)" << std::endl;
	std::cout << url << std::endl;
	
	std::cout << "loc:" << location._config_path << std::endl;

	std::string path_root = server._config_root;
	if (!location._config_root.empty() && location._config_path != "/")
	{
		std::cout << "Using location root" << std::endl;
		path_root = location._config_root;
		//remove location._config_path from url
		if (location._config_path != "/" && url.find(location._config_path) == 0)
		{
			std::string trimmed_url = url.substr(location._config_path.length());
			if (trimmed_url.empty())
				trimmed_url = "/";
			std::cout << "trimmed_url:" << trimmed_url << std::endl;
			return getPath(trimmed_url, server, location);
		}
	}
	std::cout << "path_root:" << path_root << std::endl;

	std::string path_index = server._config_index;
	if (!location._config_index.empty())
	{
		path_index = location._config_index;
	}
	std::cout << "path_index:" << path_index << std::endl;

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

	std::cout << "path 2: "<< path << std::endl;
	std::cout << "-------------------GET PATH FIN 2------------------------" << std::endl;
	return path;
}
//Verifie si la methode de la requete est autorisee dans la location
//Retourne 0 si ok, 1 sinon
int ft_check_method(const Location &loc, const Response &rep)
{
	std::vector<std::string>::const_iterator it = loc._config_allowed_methods.begin();
	for (; it != loc._config_allowed_methods.end(); it++)
	{
		std::cout << "config method:" << *it << std::endl;
		if (rep.method == *it)
			return (0);
	}

	return (1);
}

// //Parse l'url pour trouver le path_script, path_info et query_string
// //Utilise les infos de server pour trouver les extensions cgi
// //Retourne une structure ParseURL
// ParseURL parseURL(const Response &rep, const ServerConfig &server)
// {
// 	(void)rep;
// 	(void)server;
// 	ParseURL result;
// 	std::string url = rep.url;
// 	size_t query_pos = url.find('?');
// 	if (query_pos != std::string::npos)
// 	{
// 		result.url = url.substr(0, query_pos);
// 		result.query_string = url.substr(query_pos + 1);
// 	}
// 	else
// 	{
// 		result.url = url;
// 		result.query_string = "";  // Vide si pas de ? dans l'URL
// 	}
	
// 	//check tout les location pour trouver les extensions CGI
// 	for (size_t j = 0; j < server._config_location.size(); ++j)
// 	{
// 		std::vector<std::string> cgi_exts = server._config_location[j]._config_cgi_ext;
// 		if (cgi_exts.empty())
// 			continue;
		
// 		//parcours des extensions CGI pour cette location
// 		for (size_t i = 0; i < cgi_exts.size(); ++i)
// 		{
// 			std::string ext = cgi_exts[i];
// 			size_t ext_pos = result.url.find(ext);
			
// 			if (ext_pos != std::string::npos)
// 			{
// 				size_t script_end = ext_pos + ext.length();
// 				result.path_script = result.url.substr(0, script_end);
				

// 				if (script_end < result.url.length())
// 					result.path_info = result.url.substr(script_end);
// 				else
// 					result.path_info = "";
				
// 				return result;
// 			}
// 		}
// 	}
// 	// aucun cgi trouve
// 	result.path_script = result.url;
// 	result.path_info = "";

// 	return result;
// }

//Verifie la taille du body de la requete par rapport au client_max_body_size
//Retourne une chaine vide si ok, sinon retourne la reponse d'erreur 413
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
			// return "HTTP/1.1 413 Payload Too Large\r\n\r\n<h1>ERROR 413 Payload Too Large</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
			return (ft_handling_error(server, 413));
		}
	}

	if (rep.body.length() > max_size)
	{
		std::cout << "Body size exceeded" << std::endl;
		std::cout << "rep.body.length():" << rep.body.length() << std::endl;
		// return "HTTP/1.1 413 Payload Too Large\r\n\r\n<h1>ERROR 413 Payload Too Large</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
		return (ft_handling_error(server, 413));
	}

	return "";
}

//Verifie si l'url correspond a une location avec redirection
//Si oui, construit la reponse de redirection 301
//Retourne la reponse 301 ou une chaine vide si pas de redirection
std::string ft_redirection(const ServerConfig &server, ParseURL &parsed_url)
{
	for (size_t j = 0; j < server._config_location.size(); ++j)
	{
		Location loc = server._config_location[j];
		if (!loc._config_redirect.empty())
		{
			std::cout << "Checking redirection for location: " << loc._config_path << std::endl;
			std::cout << "Parsed URL for redirection check: " << parsed_url.url << std::endl;
			//if loc.redirect does not start with /, add it

			//gere les redirections avec scheme http:// or https://
			if ((loc._config_redirect.find("http://") == 0
				|| loc._config_redirect.find("https://") == 0)
				&& (parsed_url.url.find(loc._config_path) != std::string::npos
				|| (loc._config_path == "/" && parsed_url.url == "")))
			{
				std::cout << "Redirection found for location: " << loc._config_path << std::endl;

				std::string new_url = loc._config_redirect;
				//ajoute la query_string si existe
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
			else if (parsed_url.url.find(loc._config_path) != std::string::npos)
			{
				if (loc._config_redirect[0] != '/')
					loc._config_redirect = "/" + loc._config_redirect;
				std::cout << "Location redirect path: " << loc._config_redirect << std::endl;

				std::cout << "Redirection found for location: " << loc._config_path << std::endl;

				std::string new_url;
				size_t pos = parsed_url.url.find(loc._config_path);
				
				if (pos == 0)
				{
					std::string remaining = parsed_url.url.substr(loc._config_path.length());
					new_url = loc._config_redirect + remaining;
					
					size_t double_slash_pos;
					while ((double_slash_pos = new_url.find("//")) != std::string::npos)
					{
						new_url = new_url.substr(0, double_slash_pos) + new_url.substr(double_slash_pos + 1);
					}
					
					if (parsed_url.url.length() > 1 
						&& parsed_url.url[parsed_url.url.length() - 1] != '/'
						&& new_url.length() > 1 
						&& new_url[new_url.length() - 1] == '/')
					{
						new_url = new_url.substr(0, new_url.length() - 1);
					}
				}
				else
				{
					// loc._config_path n'est pas au début, simple remplacement
					new_url = parsed_url.url;
					new_url.replace(pos, loc._config_path.length(), loc._config_redirect);
				}
				std::cout << "New URL for redirection: " << new_url << std::endl;
				
				//ajoute la query_string si existe
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
		}
	}
	return "";
}

//Retourne le type mime en fonction de l'extension du fichier
//Par defaut text/plain
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

std::string Response::getRequest(Response &rep, const ServerConfig &server, Server &srv, ClientState &client_state)
{
	(void)client_state;
	std::cout << "-------------------GET REQUEST------------------------" << std::endl;

	// rep.parsed_url = parseURL(rep, server);
	rep.setParsedURL(ParseURL::ft_parseURL(rep, server));
	std::cout << "Parsed URL:" << std::endl;
	std::cout << "  Full URL: " <<  "{"<< rep.parsed_url.url << "}" << std::endl;
	std::cout << "  Script Path: " <<  "{"<< rep.parsed_url.path_script << "}" << std::endl;
	std::cout << "  Path Info: " <<  "{"<< rep.parsed_url.path_info << "}" << std::endl;
	std::cout << "  Query String: " << "{"<< rep.parsed_url.query_string << "}" << std::endl;

	//verification des redirections
	std::string test_redir = ft_redirection(server, rep.parsed_url);
	if (!test_redir.empty())
	{
		return test_redir;
	}
	Location loc = getLocation(rep.parsed_url.path_script, server);
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
				// return "HTTP/1.1 413 Payload Too Large\r\n\r\n<h1>ERROR 413 Payload Too Large</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
				return ft_handling_error(server, 413);
			}
		}
		const_cast<Response&>(rep).body = body;
		// rep.body = body;
	}
	std::cout << rep.body << std::endl;
	std::cout << "Reconstructed body length:" << rep.body.length() << std::endl;
	
	//check body size
	std::string body_size_check = ft_check_body_size(rep, server, loc);
	if (!body_size_check.empty())
		return body_size_check;

	if (!loc._config_allowed_methods.empty() && ft_check_method(loc, rep) == 1)
	{
		std::cout << "TEST5" << std::endl;
		// return "HTTP/1.1 405 Method Not Allowed\r\n\r\n<h1>ERROR 405 Method Not Allowed</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
		return (ft_handling_error(server, 405));
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

		// return "HTTP/1.1 404 Not Found\r\n\r\n<h1>ERROR 404 Not Found</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
		return (ft_handling_error(server, 404));
	}

	std::cout << "Final path to serve: " << path << std::endl;
	std::string temp_path = loc._config_path;
	// std::string temp_path = parsed_url.path_script;
	std::cout << "temp_path:" << temp_path << std::endl;

	if (temp_path.length() > 1
		&& rep.parsed_url.path_script.length() >= temp_path.length()
		&& rep.parsed_url.path_script.compare(rep.parsed_url.path_script.length() - temp_path.length(), temp_path.length(), temp_path) == 0
		&& rep.parsed_url.path_script.find(".") != std::string::npos)
	{
		std::cout << "going to handleCGI" << std::endl;
		return Resultat::handleCGI(rep, server, path, loc, rep.parsed_url, srv, client_state);
	}
	if (rep.method == "GET")
	{
		return Resultat::handleGET(path, server, loc, rep.parsed_url);
	}
	else if (rep.method == "POST")
	{
		return Resultat::handlePOST(rep, server);
	}
	else if (rep.method == "DELETE")
	{
		return Resultat::handleDELETE(rep, server, loc);
	}
	// return ("HTTP/1.1 405 Method Not ALlowed\r\n\r\n<h1>ERROR 405 Method Not Allowed</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>");
	return (ft_handling_error(server, 405));
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