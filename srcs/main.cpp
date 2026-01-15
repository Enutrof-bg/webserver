#include "../includes/webserv.hpp"
#include "../includes/Config.hpp"
#include "../includes/Server.hpp"

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cout << "Erreur: expected configuration file" << std::endl;
		return (1);
	}
	else
	{
		try
		{
			std::string s;
			if (argc == 2)
				s = argv[1];
			else
				s = "config/conf1.conf";
			Config conf(s);
			// std::cout << conf.getFilename() << std::endl;
			conf.readConfig();
			// std::cout << conf.getConfig() << std::endl;
			conf.parseConfig();
			conf.printConfig();

			Server serv(conf);
			serv.setup();
			serv.run();
		}
		catch(std::exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}
}

//http://localhost:18000/

/*
int main()
{
	int listenfd;
	int connfd;
	int n;
	sockaddr_in serverAddress;
	uint8_t buff[MAXLINE+1];
	uint8_t recvline[MAXLINE+1];
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(listenfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

	listen(listenfd, 10);

	for ( ; ; )
	{
		// sockaddr_in addr;
		// socklen_t addr_len;

		printf("Waiting for connection on port %d\n", SERVER_PORT);

		connfd = accept(listenfd, NULL, NULL);

		memset(recvline, 0, MAXLINE);

		while ((n = read(connfd, recvline, MAXLINE -1)) > 0)
		{
			printf("{%s}", recvline);

			if (recvline[n -1] == '\n')
				break;

			memset(recvline, 0, MAXLINE);
		}

		if (n < 0)
		{
			close(connfd);
			return (1);
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
*/