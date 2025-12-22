#include "../includes/Response.hpp"

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
	std::cout << "-------------rep.body:\n" <<rep.body.length() << std::endl;
	std::cout << "-------------------PARSE-REQUEST-FIN---------------------" << std::endl;
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
	if (rep.url.find("cgi-bin") != std::string::npos
		|| rep.url.find(".py") != std::string::npos
		|| rep.url.find(".php") != std::string::npos)
	{
		return handleCGI(rep, server, path);
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
	return "no method found";
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
	std::cout << rep.body  << std::endl;
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
			<< "<br><p><a title=\"Motherfucking Website\" href=\"index.html\">go back</a></p></br>\n"
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
			 << "<p><a title=\"Retour\" href=\"index.html\">go back</a></p></body>";

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
	//delete file
	if (std::remove(path.c_str()))
	{
		newbody << "<!DOCTYPE html>\n"
			<< "<html>\n<head><title>DELETE reçu</title></head>\n"
			<< "<body>\n"
			<< "<h1>Deletion didnt work "<< rep.url <<" </h1>\n"
			<< "<ul>\n";
	}
	else
	{
		newbody << "<!DOCTYPE html>\n"
			<< "<html>\n<head><title>DELETE reçu</title></head>\n"
			<< "<body>\n"
			<< "<h1>deleted "<< rep.url <<" </h1>\n"
			<< "<ul>\n";
	}

	std::ostringstream response;
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

std::string handleCGI(const Response &rep, const ServerConfig &server, const std::string &path)
{
	(void)rep;
	(void)server;
	(void)path;
	//check le path script si existe

	//pipe
	//pipe pour body
	//fork
	//processus enfant
		//redir stdout vers pipe
		//redir stdin post
		//def envp du cgi
		//executer le script
	//parent
		//envoyer le body au script si post
		//lire sortie du script
		//wait processus enfant
		//script doit return header
		//
		
	return ("caca");
}

