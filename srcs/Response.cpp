#include "../includes/Response.hpp"

//Parse la requete http et retoune une structure Response
Response parseRequest(const std::string &request)
{
	Response rep;
	rep.invalid_request = 0;

	std::cout << "-------------------PARSE REQUEST------------------------" << std::endl;
	size_t header_end = request.find("\r\n\r\n");
	if (header_end == std::string::npos)
	{
		std::cerr << "Erreur: header imcomplet" << std::endl;
		rep.invalid_request = 1;
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
	if (rep.method.empty() || rep.url.empty() || rep.version.empty())
	{
		std::cerr << "Erreur: requete invalide" << std::endl;
		rep.invalid_request = 1;
		return rep;
	}
	if (rep.version != "HTTP/1.1" && rep.version != "HTTP/1.0")
	{
		std::cerr << "Erreur: version HTTP non supportee" << std::endl;
		rep.invalid_request = 1;
		return rep;
	}
	if (rep.method != "GET" && rep.method != "POST"
		&& rep.method != "DELETE")
	{
		std::cerr << "Erreur: methode non supportee" << std::endl;
		rep.invalid_request = 1;
		return rep;
	}
	if (rep.url.find(" ") != std::string::npos)
	{
		std::cerr << "Erreur: URL invalide" << std::endl;
		rep.invalid_request = 1;
		return rep;
	}
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
	// if header ne contien pas de Host
	if (rep.header.find("Host") == rep.header.end())
	{
		std::cerr << "Erreur: header Host manquant" << std::endl;
		rep.invalid_request = 1;
		return rep;
	}
	std::cout <<"----HEADER------" << std::endl;
	std::map<std::string, std::string>::iterator it;
	for (it = rep.header.begin(); it != rep.header.end(); it++)
	{
		std::cout << "First: " <<it->first << " | "<<"Second: "<<it->second << std::endl;
	}
	// std::cout << "----BODY------" << std::endl;

	// std::cout << "-------------rep.body.length():\n" <<rep.body.length() << std::endl;
	// std::cout << "-------------------PARSE-REQUEST-FIN---------------------" << std::endl;
	return rep;
}

//Cherche la location correspondant a l'url
//Retourne la location correspondante ou la location par defaut / si definie
/*
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
*/

// On parcourt toutes les locations du serveur
// On vérifie si l'URL commence par le chemin de la location
// Si ce prefixe est plus long que le précédent trouvé
// Si on a trouvé une location spécifique, on la retourne
// On retourne si on trouve une location de cgi
// Sinon, on peut retourner une location par défaut (souvent "/" si elle existe)
// ou une location vide pour signaler qu'on utilise la config globale du serveur
Location getLocation(const std::string &url, const ServerConfig &server)
{
	Location best_match;
	size_t longest_prefix = 0;
	bool found = false;

	std::cout << "url to match location: " << url << std::endl;
	for (size_t j = 0; j < server._config_location.size(); j++)
	{
		const std::string &loc_path = server._config_location[j]._config_path;		

		// std::cout << "Checking location match, loc path: " << loc_path << std::endl;


		if (url.compare(0, loc_path.length(), loc_path) == 0
			/*&& (url[loc_path.length()] == '\0'
				|| url[loc_path.length()] == '/')*/)
		{
			if (loc_path.length() >= longest_prefix)
			{
				longest_prefix = loc_path.length();
				best_match = server._config_location[j];
				found = true;

				// std::cout << "Location matched: " << loc_path << std::endl;
			}
		}

		std::string temp_path = server._config_location[j]._config_path;
		if (temp_path.length() < url.length()
			&& temp_path.find(".") != std::string::npos)
		{
			if (url.compare(url.length() - temp_path.length(), temp_path.length(), temp_path) == 0)
				return (server._config_location[j]);
		}
	}
	if (found)
		return best_match;
	else if (!server._config_location.empty())
	{
		for (size_t j = 0; j < server._config_location.size(); j++)
		{
			// std::cout << "Checking for default location match, loc path: " << server._config_location[j]._config_path << std::endl;
			if (server._config_location[j]._config_path == "/")
			{
				return (server._config_location[j]);
			}
		}
	}
	return Location(); 
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
		// std::cout << "Using location root" << std::endl;
		path_root = location._config_root;
		//remove location._config_path from url
		std::cout << "location._config_path:" << location._config_path << std::endl;
		if (location._config_path != "/" && url.find(location._config_path) == 0)
		{
			std::string trimmed_url = url.substr(location._config_path.length());
			if (trimmed_url.empty())
				trimmed_url = "/";
			std::cout << "trimmed_url:" << trimmed_url << std::endl;
			if (trimmed_url[0] != '/')
				trimmed_url = "/" + trimmed_url;
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
		std::cout << "path before index append: "<< path << std::endl;
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

			// server._config_location[j]._config_path.length() <= temp_url.length()
			// 	&& temp_url.compare(0, server._config_location[j]._config_path.length(), server._config_location[j]._config_path) == 0
			// 	&& server._config_location[j]._config_path != "/"

			//gere les redirections avec scheme http:// or https://
			if (((loc._config_redirect.length() >= 7 //7 for "http://"
				&& loc._config_redirect.compare(0, 7, "http://") == 0)
				||(loc._config_redirect.length() >= 8 //8 for "https://"
				&& loc._config_redirect.compare(0, 8, "https://") == 0))
				&& loc._config_path.length() <= parsed_url.url.length()
				&& parsed_url.url.compare(0, loc._config_path.length(), loc._config_path) == 0
				&& (parsed_url.url[loc._config_path.length()] == '\0'
				|| parsed_url.url[loc._config_path.length()] == '/'))
			{
				std::cout << "Redirection found for location: " << loc._config_path << std::endl;

				std::string new_url = loc._config_redirect;
				//ajoute la query_string si existe
				if (parsed_url.query_string != "")
					new_url += "?" + parsed_url.query_string;

				// std::string body = "<h1>301 Moved Permanently</h1><p>The document has moved <a href=\"" + new_url + "\">here</a>.</p>";
				// std::ostringstream response;
				// response << "HTTP/1.1 301 Moved Permanently\r\n"
				// 		 << "Location: " << new_url << "\r\n"
				// 		 << "Content-Type: text/html; charset=UTF-8\r\n"
				// 		 << "Content-Length: " << body.length() << "\r\n"
				// 		 << "Connection: close\r\n"
				// 		 << "\r\n"
				// 		 << body;
				
				// return response.str();
				return ft_move_code(server, 302, new_url);
			}
			// else if (parsed_url.url.find(loc._config_path) != std::string::npos)
			else if (loc._config_path.length() <= parsed_url.url.length()
				&& parsed_url.url.compare(0, loc._config_path.length(), loc._config_path) == 0
				&& (parsed_url.url[loc._config_path.length()] == '\0'
				|| parsed_url.url[loc._config_path.length()] == '/'))
			{
				if (loc._config_redirect[0] != '/')
					loc._config_redirect = "/" + loc._config_redirect;
				std::cout << "Location redirect path: " << loc._config_redirect << std::endl;

				std::cout << "Redirection found for location: " << loc._config_path << std::endl;

				std::string new_url = loc._config_redirect;

				return ft_move_code(server, 302, new_url);
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