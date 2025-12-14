#include "../includes/Server.hpp"

Server::Server(const Config &conf): _server(conf.getServer())
{

}

Server::~Server()
{
}

void Server::setup()
{
	for(size_t i = 0; i < _server.size(); i++)
	{
		// _server[i]._config_listen
		int listenfd = socket(AF_INET, SOCK_STREAM, 0);
		if (listenfd < 0)
		{
			strerror(errno);
			continue;
		}
		_server_listen_socket.push_back(listenfd);
		std::cout << listenfd << std::endl;

		sockaddr_in serverAddress;
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(_server[i]._config_listen);
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		_server_address.push_back(serverAddress);

		int opt = 1;
		if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("setsockopt failed");

		bind(_server_listen_socket[i], (struct sockaddr *)&serverAddress, sizeof(serverAddress));

		listen(_server_listen_socket[i], 10);
	}
}

bool Server::is_listen_socket(int fd)
{
	for (size_t i = 0; i < _server_listen_socket.size(); i++)
	{
		if (_server_listen_socket[i] == fd)
			return true;
	}
	return false;
}

void Server::printListenPorts()
{
	std::cout << "=== Serveur démarré ===" << std::endl;
	std::cout << "Ports d'écoute disponibles :" << std::endl;
	for (size_t i = 0; i < _server.size(); i++)
	{
		std::cout << "  - Port " << _server[i]._config_listen 
				  << " (fd=" << _server_listen_socket[i] << ")";
		if (!_server[i]._config_server_name.empty())
			std::cout << " [" << _server[i]._config_server_name << "]";
		std::cout << std::endl;
	}
	std::cout << "En attente de connexions..." << std::endl << std::endl;
}

void Server::run()
{
	// int n;
	// uint8_t recvline[4096+1];
	// uint8_t buff[MAXLINE+1];

	printListenPorts();

	std::vector<pollfd> pollfds;
	for (size_t i = 0; i < _server_listen_socket.size(); i++)
	{
		pollfd temp;
		temp.fd = _server_listen_socket[i];
		temp.events = POLLIN;
		temp.revents = 0;
		pollfds.push_back(temp);
	}

	while(true)
	{
		int ret = poll(pollfds.data(), pollfds.size(), -1);
		if (ret < 0)
			throw std::runtime_error("Error: poll failed");
		
		for(size_t i = 0; i < pollfds.size(); i++)
		{
			if (pollfds[i].revents == 0)
				continue;

			if (is_listen_socket(pollfds[i].fd))
			{
				if (pollfds[i].revents & POLLIN)
				{
					int client_fd = accept(pollfds[i].fd, NULL, NULL);

					pollfd client_pfd;
					client_pfd.fd = client_fd;
					client_pfd.events = POLLIN;
					client_pfd.revents = 0;
					pollfds.push_back(client_pfd);

					for (size_t j = 0; j < _server_listen_socket.size(); j++)
					{
		  				if (_server_listen_socket[j] == pollfds[i].fd)
						{
			   				_client_to_server[client_fd] = j;
			 				break;
		 				}
	   				}
				}
			}
			else
			{
				if (pollfds[i].revents & POLLIN)
				{
					char buffer[4096];
					int n = read(pollfds[i].fd, buffer,	sizeof(buffer));
					printf("{%s}", buffer);
					if (n <= 0)
					{
						close(pollfds[i].fd);
						pollfds.erase(pollfds.begin() + i);
						i--;
					}
					else
					{
						Response rep = parseRequest(buffer);

						// std::string response_temp = "test";
						// std::cout << "tset" << std::endl;

						size_t server_index = _client_to_server[pollfds[i].fd];
						ServerConfig& server = _server[server_index];

						std::string response = getRequest(rep, server);
						// std::cout << response << std::endl;
						_client_responses[pollfds[i].fd] = response;
						pollfds[i].events = POLLOUT;
					}

				} 

				if (pollfds[i].revents & POLLOUT)
				{
					// const char* response = 
					// 	"<!DOCTYPE html>\n"
					// 	"<html>\n"
					// 	"<head>\n"
					// 	"	<meta charset='UTF-8'>\n"
					// 	"	<title>SilkRoad</title>\n"
					// 	"</head>\n"
					// 	"<body>\n"
					// 	"	<h1>Bonjour du serveur!</h1>\n"
					// 	"	<p>Ceci est une reponse HTML</p>\n"
					// 	"	<ul>\n"
					// 	"		<li>Port: 18000</li>\n"
					// 	"		<li>Status: OK</li>\n"
					// 	"		<li>c: 50e/g</li>\n"
					// 	"	</ul>\n"
					// 	"</body>\n"
					// 	"</html>\n";

					// char buffer[8192];
					// snprintf(buffer, sizeof(buffer),
					// 	"HTTP/1.1 200 OK\r\n"
					// 	"Content-Type: text/html; charset=UTF-8\r\n"
					// 	"Content-Length: %zu\r\n"
					// 	"Connection: close\r\n"
					// 	"\r\n"
					// 	"%s",
					// 	strlen(response), response);
					
					std::string response_2 = _client_responses[pollfds[i].fd];
					std::cout << "RESPONSE2" << std::endl;
					std::cout << response_2 << std::endl;
					write(pollfds[i].fd, response_2.c_str(), strlen(response_2.c_str()));
					//  write(pollfds[i].fd, buffer, strlen(buffer));
					_client_responses.erase(pollfds[i].fd);
					_client_to_server.erase(pollfds[i].fd);
					
					close(pollfds[i].fd);
					pollfds.erase(pollfds.begin() + i);
					i--;
				}

				if (pollfds[i].revents & (POLLHUP | POLLERR))
				{
					close(pollfds[i].fd);
					pollfds.erase(pollfds.begin() + i);
					i--;
				}
			}

		}
		// for(size_t i = 0; i < _server.size(); i++)
		// 	printf("Waiting for connection on port %d\n", _server[i]._config_listen);

		// 	int connfd = accept(_server_listen_socket[i], NULL, NULL);
			
		// 	printf("test1\n");

		// 	memset(recvline, 0, 4096);

		// 	while ((n = read(connfd, recvline, MAXLINE -1)) > 0)
		// 	{
		// 		printf("{%s}", recvline);

		// 		if (recvline[n -1] == '\n')
		// 			break;

		// 		memset(recvline, 0, MAXLINE);
		// 	}

		// 	printf("test2\n");

		// 	const char* html = 
		// 	"<!DOCTYPE html>\n"
		// 	"<html>\n"
		// 	"<head>\n"
		// 	"	<meta charset='UTF-8'>\n"
		// 	"	<title>SilkRoad</title>\n"
		// 	"</head>\n"
		// 	"<body>\n"
		// 	"	<h1>Bonjour du serveur!</h1>\n"
		// 	"	<p>Ceci est une reponse HTML</p>\n"
		// 	"	<ul>\n"
		// 	"		<li>Port: 18000</li>\n"
		// 	"		<li>Status: OK</li>\n"
		// 	"		<li>c: 50e/g</li>\n"
		// 	"	</ul>\n"
		// 	"</body>\n"
		// 	"</html>\n";
		
		// 	 snprintf((char*)buff, sizeof(buff), 
		// 	"HTTP/1.1 200 OK\r\n"
		// 	"Content-Type: text/html; charset=UTF-8\r\n"
		// 	"Content-Length: %zu\r\n"
		// 	"Connection: close\r\n"
		// 	"\r\n"
		// 	"%s", 
		// 	strlen(html), html);
		// 	// snprintf((char*)buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\n%s", strlen(html), html);

		// 	write(connfd, (char *)buff, strlen((char *)buff));

		// 	printf("test3\n");
		// 	close(connfd);

		// 	printf("test4\n");
	}
}