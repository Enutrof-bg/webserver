#include "../includes/Server.hpp"
#include <fcntl.h>
#include <sys/wait.h>

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

		if (bind(_server_listen_socket[i], (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
		{
			strerror(errno);
			continue;
		}

		if (listen(_server_listen_socket[i], 10) < 0)
		{
			strerror(errno);
			continue;
		}
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

bool Server::ft_is_fd_client_state(int fd)
{
	std::map<int, ClientState>::iterator it = _clients.begin();

	for (; it != _clients.end(); it++)
	{
		if (it->second.fd_cgi == fd)
			return true;
	}
// 	if (it != _clients.end())
// 		return true;
	return false;
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
					//cmd non bloquante
					// fcntl(client_fd, F_SETFL, O_NONBLOCK);
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
				if (ft_is_fd_client_state(pollfds[i].fd) == true)
				{
					//cgi pipe read
					std::cout << "Reading from CGI pipe fd: " << pollfds[i].fd << std::endl;
					std::map<int, ClientState>::iterator it = _clients.begin();

					for (; it != _clients.end(); it++)
					{
						if (it->second.fd_cgi == pollfds[i].fd)
							break;
					}
					if (it != _clients.end())
					{
						char buffer[4096];
						std::cout << "Reading CGI output..." << it->second.fd_cgi << std::endl;
						ssize_t n = read(it->second.fd_cgi, buffer, sizeof(buffer));
						std::cout << "Read " << n << " bytes from CGI." << std::endl;
						if (n > 0)
						{
							std::cout << "Appending " << n << " bytes to response buffer." << std::endl;
							it->second.response_buffer.append(buffer, n);
						}
						else
						{
							//fin de la lecture du cgi
							std::cout << "CGI output read complete. Closing CGI pipe fd: " << it->second.fd_cgi << std::endl;
							close(it->second.fd_cgi);
							pollfds.erase(pollfds.begin() + i);
							i--;
							waitpid(it->second.cgi_pid, NULL, WNOHANG);
							_client_responses[it->second.fd_client] = it->second.response_buffer;

							for (size_t j = 0; j < pollfds.size(); j++)
							{
								if (pollfds[j].fd == it->second.fd_client)
								{
									pollfds[j].events = POLLOUT;
									_clients[pollfds[i].fd].state = ClientState::WRITING_RES;
									break;
								}
							}
							
							// _clients.state
							// _clients.erase(it);
						}
					}
				}
				else if (pollfds[i].revents & POLLIN)
				{

					// char buffer[4096];
					// char buffer[100000];
					// int n = read(pollfds[i].fd, buffer,	sizeof(buffer));
					// std::cout << "=============REQUEST-IN============" << std::endl;
					// printf("{%s}\n", buffer);
					// std::cout << "=============END-REQUEST-IN=========" << std::endl;
		
			// std::cout << "Premiers octets (hex): ";
			// for (size_t i = 0; i < 50000 && i <100000; i++)
			// {
			// 	printf("%02X ", (unsigned char)buffer[i]);
			// 	// printf(":%zu | ", i);
			// }
			// std::cout << std::endl;
					std::cout << "=================================123==" << std::endl;



					std::string request;
					char buffer[4096];
					ssize_t n;
					n = recv(pollfds[i].fd, buffer, sizeof(buffer), 0);
				
						printf("n:%ld\n", n);
					if (n > 0)
					{
						printf("========BUFFER:%s\n", buffer);
						request.append(buffer, n);
						printf("========request:%s\n", request.c_str());
					}
						// n = read(pollfds[i].fd, buffer, sizeof(buffer));
						// printf("n:%ld\n", n);
						size_t header_end = request.find("\r\n\r\n");
						if (header_end != std::string::npos)
						{
								//gestion content-length
								size_t cl_pos = request.find("Content-Length:");
								if (cl_pos != std::string::npos)
								{
									size_t cl_end = request.find("\r\n", cl_pos);
									std::string cl_str = request.substr(cl_pos + 15, cl_end - (cl_pos + 15));
									size_t content_length = std::atoi(cl_str.c_str());
									
									size_t total_expected = header_end + 4 + content_length;
									
									while (request.length() < total_expected)
									{
										n = recv(pollfds[i].fd, buffer, sizeof(buffer), 0);
										if (n <= 0)
											break;
										request.append(buffer, n);
									}
									// break;
								}
							// }
						}
					// }
					printf("im out\n");
					if (n <= 0)
					{
						close(pollfds[i].fd);
						pollfds.erase(pollfds.begin() + i);
						i--;
					}
					else
					{
						Response rep = parseRequest(request);

						// std::string response_temp = "test";
						// std::cout << "tset" << std::endl;

						size_t server_index = _client_to_server[pollfds[i].fd];
						ServerConfig& server = _server[server_index];

						std::map<int,ClientState>::iterator it_client = _clients.find(pollfds[i].fd);
						if (it_client == _clients.end())
						{

							ClientState new_client;
							new_client.fd_client = pollfds[i].fd;
							new_client.last_activity = time(NULL);
							_clients[pollfds[i].fd] = new_client;

							
						}

						std::string response = getRequest(rep, server, *this, _clients[pollfds[i].fd]);
						// std::cout << response << std::endl;
						
						if (_clients[pollfds[i].fd].state == ClientState::READING_CGI)
						{
							pollfd _pipe_cgi_fd;
							_pipe_cgi_fd.fd = _clients[pollfds[i].fd].fd_cgi;
							_pipe_cgi_fd.events = POLLIN;
							_pipe_cgi_fd.revents = 0;
							pollfds.push_back(_pipe_cgi_fd);

							pollfds[i].events = 0;
						}
						else
						{
							_client_responses[pollfds[i].fd] = response;
							pollfds[i].events = POLLOUT;
						}
						// _client_responses[pollfds[i].fd] = response;
						// pollfds[i].events = POLLOUT;
					}
				} 

				if (pollfds[i].revents & POLLOUT)
				{
					std::string response_2 = _client_responses[pollfds[i].fd];
					std::cout << "============================================================" << std::endl;
					std::cout << "----RESPONSE-SENT-TO-CLIENT----" << std::endl;
					if (_clients[pollfds[i].fd].state == ClientState::WRITING_RES)
					{
						_clients[pollfds[i].fd].state = ClientState::IDLE;

						// std::string response_header = "HTTP/1.1 200 OK\r\n";
						// response_header += "Content-Length: " + response_2.length() + "\r\n";
						// response_header += "Connection: close\r\n";
						// response_header += "\r\n";
						// response_2 = response_header + response_2;
						std::ostringstream response;
						response << "HTTP/1.1 200 OK\r\n"
								<<"Content-Type: " << "text/html" << "; charset=UTF-8\r\n"
								<<"Content-Length: " << response_2.length()<<"\r\n"
								<<"Connection: close\r\n"
								<<"\r\n"
								<< response_2;
						response_2 = response.str();
					}

					std::cout << response_2 << std::endl;
				write(pollfds[i].fd, response_2.c_str(), response_2.length());
					std::cout << "----END-OF-RESPONSE-SENT-TO-CLIENT----" << std::endl;
					std::cout << "============================================================\n\n\n\n\n" << std::endl;
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
	}
}