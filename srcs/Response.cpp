#include "../includes/Response.hpp"
#include <sys/wait.h>

inline std::string& rtrim(std::string& s, const char* t)
{
	size_t pos = s.find_last_not_of(t);
	if (pos != std::string::npos)
		s.erase(pos + 1);
	else
		s.clear();
    return s;
}

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
    //                        std::istreambuf_iterator<char>());
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
	return Location();
}

std::string getPath(const std::string &url, const ServerConfig &server)
{
	// return "asd";
	if (url.empty())
		std::cout << "ERREUR" << std::endl;
	std::cout << url << std::endl;
	std::string temp_url(url);
	temp_url = rtrim(temp_url, "/");
	
	// location._config_root = rtrim(location._config_root, " \t;");
	if (temp_url.size() > 1)
	{
		for (size_t j = 0; j < server._config_location.size(); ++j)
		{
			if (server._config_location[j]._config_path.find(temp_url) != std::string::npos)
			{
				std::string path = server._config_location[j]._config_root;

				if (url == "/" || url[url.length() - 1] == '/')
				{
					// path = path + url + server._config_location[j]._config_index;
					path = path + "/" + server._config_location[j]._config_index;

					std::cout << path << std::endl;
					std::cout << "LOCATION2" << std::endl;
				}
				else
				{
					path = path + "/" +server._config_location[j]._config_index;
					std::cout << path << std::endl;
					std::cout << "LOCATION1" << std::endl;
				}
				std::cout << "LOCATION" << std::endl;
				return path;
			}
		}
	}
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

ParseURL parseURL(const std::string &url, const Response &rep)
{
	(void)rep;  // Le body n'est pas utilisé ici !
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
	
	size_t script_end = result.url.find(".py");
	if (script_end != std::string::npos)
	{
		script_end += 3; // length of ".py"
		result.path_script = result.url.substr(0, script_end);
		result.path_info = result.url.substr(script_end);
	}
	else
	{
		result.path_script = result.url;
		// result.path_info = std::string();
		result.path_info = "";
	}
	
	return result;
}

std::string getRequest(const Response &rep, const ServerConfig &server)
{
	//error a gerer check method
	// (void)server;
	// return "caca1";
	// std::cout << rep.url << std::endl;
	ParseURL parsed_url = parseURL(rep.url, rep);
	std::cout << "Parsed URL:" << std::endl;
	std::cout << "  Full URL: " <<  "{"<< parsed_url.url << "}" << std::endl;
	std::cout << "  Script Path: " <<  "{"<< parsed_url.path_script << "}" << std::endl;
	std::cout << "  Path Info: " <<  "{"<< parsed_url.path_info << "}" << std::endl;
	std::cout << "  Query String: " << "{"<< parsed_url.query_string << "}" << std::endl;

	Location loc = getLocation(parsed_url.path_script, server);
	std::cout << "loc path:"<<loc._config_path << std::endl;
	std::cout << "method size:"<<loc._config_allowed_methods.size() << std::endl;
	std::vector<std::string>::const_iterator it = loc._config_allowed_methods.begin();
	for (; it != loc._config_allowed_methods.end(); it++)
	{
			std::cout << "config method:" << *it << std::endl;
	}

	std::cout << "TEST1" << std::endl;

	if (!loc._config_allowed_methods.empty() && ft_check_method(loc, rep) == 1)
	{
		std::cout << "TEST5" << std::endl;
		return "HTTP/1.1 405 Method Not Allowed\r\n\r\n<h1>ERROR 405 Method Not Allowed</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
	}
	std::cout << "TEST6" << std::endl;
	std::string path = getPath(rep.url, server);

	
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
		return handleCGI(rep, server, path, loc, parsed_url);
	}
	if (rep.method == "GET")
	{
		return handleGET(path, server);
	}
	else if (rep.method == "POST")
	{
		return handlePOST(rep, server);
	}
	else if (rep.method == "DELETE")
	{
		return handleDELETE(rep, server);
	}
	// return "no method found";
	return ("HTTP/1.1 405 Method Not ALlowed\r\n\r\n<h1>ERROR 405 Method Not Allowed</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>");
}

std::string handleGET(const std::string &path, const ServerConfig &server)
{
	(void)server;
	std::cout << "-----------------------------------HANDLE_GET----------------" <<std::endl;
	std::cout << "Chemin apth: " << path << std::endl;
	
	std::string line;
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

std::string handleDELETE(const Response &rep, const ServerConfig &server)
{
	(void)rep;
	(void)server;
	//path
	std::cout << "-----------------------------------HANDLE_DELETE-------------" <<std::endl;
	std::cout << "URL1:" <<rep.url.c_str() << std::endl;
	std::string path = getPath(rep.url.c_str(), server);
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
	if ((rep.header.find("Content-Length")) != rep.header.end())
	newenv = ft_add_double_tab(const_cast<char*>(("CONTENT_LENGTH=" + rep.header.find("Content-Length")->second).c_str()), newenv);
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
						std::string path, const Location &loc, const ParseURL &parsed_url)
{
	(void)rep;
	(void)server;
	(void)path;
	(void)loc;
	(void)parsed_url;
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

		//wait processus enfant
		int status;
		waitpid(id, &status, 0);
		// close(scriptfd[0]);
		close(scriptfd[1]);
		//envoyer le body au script si post
		char buffer[4096];
		read(scriptfd[0], buffer, sizeof(buffer));

		close(scriptfd[0]);
		//lire sortie du script

	
		
		//script doit return header

		//script doit return header
		//
		std::cout << "\n[BUFFER:"<< buffer << "]"<< std::endl;
		std::cout << "================parent==FIN===============" << std::endl;

	
		if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
		{
			return "HTTP/1.1 500 Internal Server Error\r\n\r\n<h1>ERROR 500 Execve Error</h1>";
		}

		std::ostringstream response;
		response << "HTTP/1.1 200 OK\r\n"
					 << buffer;
		return response.str();
	}
	return "HTTP/1.1 500 Internal Server Error\r\n\r\n<h1>ERROR 500 Server error</h1><p><a title=\"GO BACK\" href=\"/\">go back</a></p>";
	// return ("caca");
}

