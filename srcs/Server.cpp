#include "../includes/Server.hpp"
#include <fcntl.h>

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

void ft_print_map(std::map<int, int> mp)
{
	std::cout << "Printing map contents:" << std::endl;
	for (std::map<int, int>::iterator it = mp.begin(); it != mp.end(); ++it)
	{
		std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
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

bool Server::is_cgi_pipe_socket_second(int fd)
{
	std::cout << "Checking if fd: " << fd << " is a CGI pipe socket." << std::endl;
	std::map<int, int>::iterator it;
	for (it = _cgi_pipe_client.begin(); it != _cgi_pipe_client.end(); it++)
	{
		std::cout << "Checking CGI pipe socket fd: " << it->second << " against fd: " << fd << std::endl;
		if (it->second == fd)
		{
			std::cout << "fd: " << fd << " is a CGI pipe socket." << std::endl;
			return true;
		}
	}
	std::cout << "fd: " << fd << " is not a CGI pipe socket." << std::endl;
	return false;
}

bool Server::is_cgi_pipe_socket(int fd)
{
	std::cout << "Checking if fd: " << fd << " is a CGI pipe socket." << std::endl;
	std::map<int, int>::iterator it;
	for (it = _cgi_pipe_client.begin(); it != _cgi_pipe_client.end(); it++)
	{
		std::cout << "Checking CGI pipe socket fd: " << it->first << " against fd: " << fd << std::endl;
		if (it->first == fd)
		{
			std::cout << "fd: " << fd << " is a CGI pipe socket." << std::endl;
			return true;
		}
	}
	std::cout << "fd: " << fd << " is not a CGI pipe socket." << std::endl;
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

	// std::vector<pollfd> pollfds;
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
				if (pollfds[i].revents & POLLIN)
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
/*
					if (is_cgi_pipe_socket_second(pollfds[i].fd))
					{
						//handle cgi pipe read
						std::cout << "Handling CGI pipe read for fd: " << pollfds[i].fd << std::endl;
						std::map<int, int>::iterator it;
						int client_fd = -1;
						for (it = _cgi_pipe_client.begin(); it != _cgi_pipe_client.end(); it++)
						{
							std::cout << "CGI pipe client map - Key: " << it->first << ", Value: " << it->second << std::endl;
							if (it->second == pollfds[i].fd)
							{
								std::cout << "Found CGI pipe client for fd: " << pollfds[i].fd << std::endl;
								client_fd = it->first;
								break;
							}

						}

						std::cout << "=================================1234==" << std::endl;
						// it = _cgi_pipe_client.find(pollfds[i].fd);
						// if (it != _cgi_pipe_client.end())
						// {
						// 	std::cout << "Found CGI pipe client for fd: " << pollfds[i].fd << std::endl;
							// int server_index = it->first;
							// ServerConfig& server = _server[server_index];

							char cgi_buffer[4096];
							ssize_t cgi_n = read(_cgi_pipe_client[it->second], cgi_buffer, sizeof(cgi_buffer));
							if (cgi_n > 0)
							{
								std::cout << "Read " << cgi_n << " bytes from CGI pipe." << std::endl;
								std::string cgi_output(cgi_buffer, cgi_n);
								_client_responses[client_fd] += cgi_output;
							}
							else
							{
								std::cout << "CGI pipe closed for fd: " << pollfds[i].fd << std::endl;
								std::string response_cgi = _client_responses[client_fd];
								close(pollfds[i].fd);
								pollfds.erase(pollfds.begin() + i);
								_cgi_pipe_client.erase(it);
								i--;

								// _client_responses[pollfds[i].fd] = response;
								for (size_t j = 0; j < pollfds.size(); j++)
								{
									std::cout << "Checking pollfd fd: " << pollfds[j].fd << " against client fd: " << client_fd << std::endl;
									if (pollfds[j].fd == client_fd)
									{
										// _client_responses[pollfds[j].fd] = response_cgi;
										std::cout << "Storing CGI response for client fd: " << pollfds[j].fd << std::endl;
										_client_responses[pollfds[j].fd] = response_cgi;
										pollfds[j].events = POLLOUT;
									}
								}

								// pollfds[i].events = POLLOUT;

							}
						// }
					}
*/
					// else
					// {
						std::cout << "=================================123==" << std::endl;

						std::string request;
						char buffer[4096];
						ssize_t n;
						n = recv(pollfds[i].fd, buffer, sizeof(buffer), 0);
					
							printf("n:%ld\n", n);
							printf("========BUFFER:%s\n", buffer);
							request.append(buffer, n);
							printf("========request:%s\n", request.c_str());
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

							actual_port = pollfds[i].fd;
							std::string response = getRequest(rep, server, *this);

							//parse _cgi_pipe_client to get cgi output
							// std::map<int, int>::iterator it_cgi;
							// for (it_cgi = _cgi_pipe_client.begin(); it_cgi != _cgi_pipe_client.end(); ++it_cgi)
							// {

							// }

							// std::cout << response << std::endl;
							std::cout << "==============================NORMALtest=========================" << std::endl;
							if (is_cgi_pipe_socket(pollfds[i].fd) == false)
							{
								std::cout << "Storing response for client fd: " << pollfds[i].fd << std::endl;
								_client_responses[pollfds[i].fd] = response;
								pollfds[i].events = POLLOUT;
							}
							else
							{
								std::cout << "CGI response pending for client fd: " << pollfds[i].fd << std::endl;
								pollfds[i].events = 0;
							}
							
						}
					// } 
				}

				if (pollfds[i].revents & POLLOUT)
				{
					std::cout << "Sending response to client fd: " << pollfds[i].fd << std::endl;
					std::string response_2 = _client_responses[pollfds[i].fd];
					std::cout << "============================================================" << std::endl;
					std::cout << "----RESPONSE-SENT-TO-CLIENT----" << std::endl;
					std::cout << response_2 << std::endl;

			// 		std::cout << "Premiers octets (hex): ";
			// for (size_t i = 0; i < 50000 && i <100000; i++)
			// {
			// 	printf("%02X ", (unsigned char)response_2[i]);
			// 	// printf(":%zu | ", i);
			// }
			// std::cout << std::endl;

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