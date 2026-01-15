#include "../includes/Resultat.hpp"

Resultat::Resultat()
{}

Resultat::Resultat(std::string message) : _message(message)
{}

std::string Resultat::getMessage() const
{
	return _message;
}

void Resultat::setMessage(std::string message)
{
	_message = message;
}

std::string Resultat::handleGET(const std::string &path, const ServerConfig &server, const Location &loc, const ParseURL &parsed_url)
{
	(void)server;
	(void)loc;
	(void)parsed_url;
	std::cout << "-----------------------------------HANDLE_GET----------------" <<std::endl;
	std::cout << "Chemin apth: " << path << std::endl;
	std::cout << "parsed url: " << parsed_url.url << std::endl;
	std::cout << "loc autoindex: " << loc._config_autoindex << std::endl;
	std::cout << "loc config_path: " << loc._config_path << std::endl;
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
			return (ft_handling_error(server, 403));
		}
	}
	
	std::string line;
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "File not found, checking autoindex..." << std::endl;
		if (loc._config_autoindex == true)
		{
			std::cout << "Autoindex activé, listing le répertoire parent..." << std::endl;
			std::string dir_path;
			if (!loc._config_root.empty())
				dir_path = loc._config_root;
			else
				dir_path = server._config_root;
			std::cout << "Root path for autoindex: " << dir_path << std::endl;

			std::cout << "parsed_url.url before modification: " << parsed_url.url << std::endl;
			//remove loc._config_path from parsed_url.url
			if (loc._config_path != "/" && parsed_url.url.find(loc._config_path) == 0)
			{
				std::string trimmed_url = parsed_url.url.substr(loc._config_path.length());
				if (trimmed_url.empty())
					trimmed_url = "/";
				std::cout << "trimmed_url for autoindex: " << trimmed_url << std::endl;
				if (dir_path[dir_path.length() - 1] != '/' && trimmed_url[0] != '/')
					dir_path += "/";
				dir_path += trimmed_url;
			}
			// append parsed_url.url to dir_path if loc._config_path is /
			else if (loc._config_path == "/")
			{
				if (dir_path[dir_path.length() - 1] != '/' && parsed_url.url[0] != '/')
					dir_path += "/";
				dir_path += parsed_url.url;
			}

			std::cout << "Directory path for autoindex: " << dir_path << std::endl;
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
		return (ft_handling_error(server, 404));
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


//Gere les requetes POST
//Si content-type est application/x-www-form-urlencoded, parse les donnees et cree une page html de reponse
//Si content-type est multipart/form-data, parse les donnees et affiche dans la console les parties recues
//Retourne la reponse HTTP
std::string Resultat::handlePOST(const Response &rep, const ServerConfig &server)
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

		for (std::map<std::string, std::string>::iterator it = data.begin(); it != data.end(); it++)
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
			// for (size_t i = 0; i < 20 && i < part_body.length(); i++)
			// {
			// 	printf("%02X ", (unsigned char)part_body[i]);
			// }
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
	return "PROUST";
}



std::string Resultat::handleDELETE(const Response &rep, const ServerConfig &server, Location &loc)
{
	(void)rep;
	(void)server;
	std::cout << "-----------------------------------HANDLE_DELETE-------------" <<std::endl;
	std::cout << "URL1:" <<rep.url.c_str() << std::endl;
	std::string path = getPath(rep.url.c_str(), server, loc);
	std::cout << "URL2:" << path << std::endl;
	// path = "../" + path;
	// std::cout << "URL3:" << path << std::endl;
	std::ostringstream newbody;
	std::ostringstream response;

	//path end with '/'
	if (path[path.length() -1] == '/')
	{
		// newbody << "<!DOCTYPE html>\n"
		// 	<< "<html>\n<head><title>DELETE reçu</title></head>\n"
		// 	<< "<body>\n"
		// 	<< "<h1>Deletion didnt work "<< rep.url <<" </h1>\n"
		// 	<< "<ul>\n";
		// response << "HTTP/1.1 400 Bad Request\r\n"
		// 	<< "Content-Type: text/html; charset=UTF-8\r\n"
		// 	<< "Content-Length: " << newbody.str().length() << "\r\n"
		// 	<< "Connection: close\r\n"
		// 	<< "\r\n"
		// 	<< newbody.str();
		// std::cout << "-----------------------------------HANDLE_DELETE-FIN---------" <<std::endl;
		// return response.str();
		return (ft_handling_error(server, 400));
	}

	//delete file
	if (std::remove(path.c_str()))
	{
		// newbody << "<!DOCTYPE html>\n"
		// 	<< "<html>\n<head><title>DELETE reçu</title></head>\n"
		// 	<< "<body>\n"
		// 	<< "<h1>Deletion didnt work "<< rep.url <<" </h1>\n"
		// 	<< "<ul>\n";
		// response << "HTTP/1.1 404 Not Found\r\n"
		// 	<< "Content-Type: text/html; charset=UTF-8\r\n"
		// 	<< "Content-Length: " << newbody.str().length() << "\r\n"
		// 	<< "Connection: close\r\n"
		// 	<< "\r\n"
		// 	<< newbody.str();
		// std::cout << "-----------------------------------HANDLE_DELETE-FIN---------" <<std::endl;
		// return response.str();
		return (ft_handling_error(server, 404));
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
}


std::string Resultat::handleCGI(const Response &rep, const ServerConfig &server,
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

	std::cout << "PathCGI1:" << path << std::endl;

	std::string temp_cgi_path = loc._config_cgi_path;
	std::cout << "PathCGI2:" << temp_cgi_path << std::endl;

	//check le path script si existe
	std::ifstream script_path;
	script_path.open(temp_cgi_path.c_str());
	if (!script_path.is_open())
	{
		return (ft_handling_error(server, 404));
		// return "HTTP/1.1 404 Not Found\r\n\r\n<h1>Script not found</h1><ap><a title=\"GO BACK\" href=\"/\">go back</a>";
	}
	script_path.close();

	//pipe
	int scriptfd[2];
	if (pipe(scriptfd) < 0)
	{
		// return "HTTP/1.1 500Internal Server Error\r\n\r\n<h1>ERROR 500 Pipe Error</h1><a title=\"GO BACK\" href=\"/\">go back</a>";
		return (ft_handling_error(server, 500));
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
			// return "HTTP/1.1 500 Internal Server Error\r\n\r\n<h1>ERROR 500 Pipe Error</h1><ap><a title=\"GO BACK\" href=\"/\">go back</a>";
			return (ft_handling_error(server, 500));
		}
	}

	//fork
	int id = fork();
	if (id < 0)
	{
		//PROTEC
		// return "HTTP/1.1 500 Internal Server Error\r\n\r\n<h1>ERROR 500 Fork Error</h1><ap><a title=\"GO BACK\" href=\"/\">go back</a>";
		return (ft_handling_error(server, 500));
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
	
		close(scriptfd[1]);
		
		std::cout << "================parent==READ_FIN===============" << std::endl;

		std::cout << "================parent==FIN===============" << std::endl;

		if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
		{
			// return "HTTP/1.1 500 Internal Server Error\r\n\r\n<h1>ERROR 500 Execve Error</h1>";
			return (ft_handling_error(server, 500));
		}

		std::string response = "";
		// std::ostringstream response;
		// response << "HTTP/1.1 200 OK\r\n"
					//  << buff_output;
		return response;
	}
	// return "HTTP/1.1 500 Internal Server Error\r\n\r\n<h1>ERROR 500 Server error</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
	return (ft_handling_error(server, 500));
	// return ("caca");
}

