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
		_server_listen_socket.push_back(listenfd);

		sockaddr_in serverAddress;
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(_server[i]._config_listen);
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		_server_address.push_back(serverAddress);

		bind(_server_listen_socket[i], (struct sockaddr *)&serverAddress, sizeof(serverAddress));

		listen(_server_listen_socket[i], 10);
	}
}

void Server::run()
{
	int n;
	uint8_t recvline[4096+1];
	uint8_t buff[MAXLINE+1];
	for (;;)
	{
		for(size_t i = 0; i < _server.size(); i++)
		{
			printf("Waiting for connection on port %d\n", _server[i]._config_listen);

			int connfd = accept(_server_listen_socket[i], NULL, NULL);

			memset(recvline, 0, 4096);

			while ((n = read(connfd, recvline, MAXLINE -1)) > 0)
			{
				printf("{%s}", recvline);

				if (recvline[n -1] == '\n')
					break;

				memset(recvline, 0, MAXLINE);
			}
			const char* html = 
			"<!DOCTYPE html>\n"
			"<html>\n"
			"<head>\n"
			"	<meta charset='UTF-8'>\n"
			"	<title>SilkRoad</title>\n"
			"</head>\n"
			"<body>\n"
			"	<h1>Bonjour du serveur!</h1>\n"
			"	<p>Ceci est une reponse HTML</p>\n"
			"	<ul>\n"
			"		<li>Port: 18000</li>\n"
			"		<li>Status: OK</li>\n"
			"		<li>c: 50e/g</li>\n"
			"	</ul>\n"
			"</body>\n"
			"</html>\n";
		
			 snprintf((char*)buff, sizeof(buff), 
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html; charset=UTF-8\r\n"
			"Content-Length: %zu\r\n"
			"Connection: close\r\n"
			"\r\n"
			"%s", 
			strlen(html), html);
			// snprintf((char*)buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\n%s", strlen(html), html);

			write(connfd, (char *)buff, strlen((char *)buff));
			close(connfd);
		}
	}
}