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
		int listenfd = socket(AF_INET, SOCK_STREAM, 0);
		if (listenfd < 0)
		{
			strerror(errno);
			continue;
		}
		_server_listen_socket.push_back(listenfd);
		// std::cout << listenfd << std::endl;

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
			// std::cerr << "Bind failed: " << strerror(errno) << std::endl;
			throw std::runtime_error("Bind failed");
			// continue;
		}

		if (listen(_server_listen_socket[i], 1024) < 0)
		{
			// std::cerr << "Listen failed: " << strerror(errno) << std::endl;
			throw std::runtime_error("Listen failed");
			// continue;
		}
	}
}

bool Server::is_listen_socket(int fd)
{
	for (size_t i = 0; i < _server_listen_socket.size(); i++)
	{
		// std::cout << "Checking listen socket: " << _server_listen_socket[i] << " against fd: " << fd << std::endl;
		if (_server_listen_socket[i] == fd)
		{
			// std::cout << "Match found for listen socket fd: " << fd << std::endl;
			return true;
		}
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
		// std::cout << "Checking client state for fd: " << fd << " against client fd: " << it->second.fd_client << " and cgi fd: " << it->second.fd_cgi << std::endl;
		if (it->second.fd_cgi == fd && it->second.fd_cgi != -1)
		{
			std::cout << "Match found for CGI pipe fd: " << fd << std::endl;
			return true;
		}
	}
	return false;
}

bool Server::ft_is_timeout_over(int fd)
{
	std::map<int, ClientState>::iterator it = _clients.begin();

	for (; it != _clients.end(); it++)
	{
		if (it->first == fd && it->second.state == ClientState::READING_CGI)
		{
			time_t now = time(NULL);
			std::cout << "Check timer ... " << std::endl;
			if (difftime(now, it->second.last_activity) > TIMEOUT_SECONDS)
			{
				std::cout << "TIMEOUT ..." << std::endl;
				return true;
			}
		}
	}
	return false;
}

void Server::ft_remove_fd(int fd)
{
	for(size_t i = 0; i < pollfds.size(); i++)
	{
		if (pollfds[i].fd == fd)
		{
			// close(pollfds[i].fd);
			std::cout << "Removing fd from pollfds: " << fd << std::endl;
			pollfds.erase(pollfds.begin() + i);
		}
	}
}
bool Server::ft_is_timeout(int fd)
{
	std::map<int, ClientState>::iterator it = _clients.begin();
	for (; it != _clients.end(); it++)
	{
		if (it->first == fd && it->second.state == ClientState::TIMEOUT)
			return true;
	}
	return false;
}

void Server::ft_check_timeout()
{
	while (waitpid(-1, NULL, WNOHANG) > 0)
	{
	}

	std::map<int, ClientState>::iterator it = _clients.begin();

	while (it != _clients.end())
	{
		time_t now = time(NULL);
		
		//timeout pour les clients en lecture de requete
		if (it->second.state == ClientState::READING_REQ)
		{
			if (difftime(now, it->second.last_activity) > TIMEOUT_SECONDS)
			{
				std::cout << "Client Request Timeout for fd: " << it->second.fd_client << std::endl;
				
				//fermer et retirer le fd du poll
				for(size_t i = 0; i < pollfds.size(); i++)
				{
					if (pollfds[i].fd == it->second.fd_client)
					{
						close(pollfds[i].fd);
						pollfds.erase(pollfds.begin() + i);
						break;
					}
				}
				_client_to_server.erase(it->second.fd_client);
				std::map<int, ClientState>::iterator to_erase = it;
				++it;
				_clients.erase(to_erase);
				continue;
			}
		}
		//timeout pour les CGI
		else if (it->second.state == ClientState::READING_CGI)
		{
			if (difftime(now, it->second.last_activity) > TIMEOUT_SECONDS)
			{
				std::cout << "Time:" << difftime(now, it->second.last_activity) << std::endl;
				// if (it->second.cgi_pid != -1)
				// {
					kill(it->second.cgi_pid, SIGKILL);
					waitpid(it->second.cgi_pid, NULL, WNOHANG);
				// }
				std::cout << "CGI Timeout detected for fd: " << it->second.fd_cgi << std::endl;
				it->second.response_buffer = "HTTP/1.1 504 Gateway Timeout\r\n\r\n<h1>ERROR 504 CGI Timeout</h1><ap><a title=\"GO BACK\" href=\"/\">go back</a></p>";
				_client_responses[it->second.fd_client] = it->second.response_buffer;

				close(it->second.fd_cgi);

				for(size_t i = 0; i < pollfds.size(); i++)
				{
					if (pollfds[i].fd == it->second.fd_cgi)
					{
						std::cout << "Removing CGI fd from pollfds due to timeout: " << it->second.fd_cgi << std::endl;
						pollfds.erase(pollfds.begin() + i);

						// pollfds[i].fd = 0;
						// waitpid(it->second.cgi_pid, NULL, WNOHANG);
						break ;
						
					}
				}
				it->second.fd_cgi = -1;
				
				for(size_t i = 0; i < pollfds.size(); i++)
				{
					if (pollfds[i].fd == it->second.fd_client)
					{
						std::cout << "Setting client fd " << it->second.fd_client << " to POLLOUT due to timeout." << std::endl;
						pollfds[i].events = POLLOUT;
						it->second.state = ClientState::TIMEOUT;

						std::cout << "Client marked for timeout response." << std::endl;
						break;
					}
				}			
			}
		}
		++it;
	}
}

// Location Server::getLocation(const std::string &path, const ServerConfig &server)
// {
// 	Location best_match;
// 	size_t best_length = 0;

// 	for (size_t i = 0; i < server._locations.size(); i++)
// 	{
// 		const Location &loc = server._locations[i];
// 		if (path.compare(0, loc._config_path.length(), loc._config_path) == 0)
// 		{
// 			if (loc._config_path.length() > best_length)
// 			{
// 				best_length = loc._config_path.length();
// 				best_match = loc;
// 			}
// 		}
// 	}
// 	return best_match;
// }

bool Server::ft_check_body_size(int fd, const ClientState &client)
{
	Location loc;
	size_t server_index = _client_to_server[fd];
	ServerConfig& server = _server[server_index];
	Response rep = parseRequest(client.request_buffer);
	rep.setParsedURL(ParseURL::ft_parseURL(rep, server));
	loc = getLocation(rep.parsed_url.path_script, server);
	std::cout << "Location found for body size check: " << loc._config_path << std::endl;
	size_t max_size = server._config_client_max_body_size;
	if (loc._config_client_max_body_size > 0)
		max_size = loc._config_client_max_body_size;
	std::cout << "Current request buffer size: " << client.request_buffer.length() << ", Max allowed size: " << max_size << std::endl;
	if (client.request_buffer.length() > max_size)
	{
		return true;
	}
	return false;

}
// Location loc;
// size_t server_index = _client_to_server[pollfds[i].fd];
// ServerConfig& server = _server[server_index];
// Response rep = parseRequest(client.request_buffer);
// rep.setParsedURL(ParseURL::ft_parseURL(rep, server));
// loc = getLocation(rep.parsed_url.path_script, server);
// std::cout << "Location found for body size check: " << loc._config_path << std::endl;
// size_t max_size = server._config_client_max_body_size;
// if (loc._config_client_max_body_size > 0)
// 	max_size = loc._config_client_max_body_size;
// if (client.request_buffer.length() > max_size)

void Server::run()
{
	printListenPorts();

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
		int ret = poll(pollfds.data(), pollfds.size(), 0); // timeout de 1000 ms
		if (ret < 0)
			throw std::runtime_error("Error: poll failed");
		
		ft_check_timeout();

		for(size_t i = 0; i < pollfds.size(); i++)
		{
			if (pollfds[i].revents == 0)
			{
				// std::cout << "No events for fd: " << pollfds[i].fd << std::endl;
				continue;
			}

			if (is_listen_socket(pollfds[i].fd))
			{
				// std::cout << "New connection on listen socket fd: " << pollfds[i].fd << std::endl;
				if (pollfds[i].revents & POLLIN)
				{
					std::cout << "Accepting new client connection..." << std::endl;

					int client_fd = accept(pollfds[i].fd, NULL, NULL);
					fcntl(client_fd, F_SETFL, O_NONBLOCK);
					std::cout << "New client connected with fd: " << client_fd << std::endl;
					pollfd client_pfd;
					client_pfd.fd = client_fd;
					client_pfd.events = POLLIN;
					client_pfd.revents = 0;
					pollfds.push_back(client_pfd);

					for (size_t j = 0; j < _server_listen_socket.size(); j++)
					{
		  				if (_server_listen_socket[j] == pollfds[i].fd)
						{
							std::cout << "Mapping client fd " << client_fd << " to server index " << j << std::endl;
			   				_client_to_server[client_fd] = j;
			 				break;
		 				}
	   				}
					// continue;
					// i--;
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
							int status = 0;
							waitpid(it->second.cgi_pid, &status, WNOHANG);
							if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
							{
								// return "HTTP/1.1 500 Internal Server Error\r\n\r\n<h1>ERROR 500 Execve Error</h1>";
								std::cerr << "CGI script execution failed." << std::endl;
								_client_responses[it->second.fd_client] = 
									ft_handling_error(_server[_client_to_server[it->second.fd_client]], 500);
								// return (ft_handling_error(*this, 500));
								for (size_t j = 0; j < pollfds.size(); j++)
								{
									if (pollfds[j].fd == it->second.fd_client)
									{
										pollfds[j].events = POLLOUT;
										_clients[pollfds[i].fd].state = ClientState::FAILED_CGI;
										break;
									}
								}
							}
							else
							{
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
							}

							
						}
					}
				}
				else if (pollfds[i].revents & POLLIN)
				{
					std::cout << "=================================123==" << std::endl;

					//initialiser le client si necessaire
					std::map<int,ClientState>::iterator it_client = _clients.find(pollfds[i].fd);
					if (it_client == _clients.end())
					{
						ClientState new_client;
						new_client.fd_client = pollfds[i].fd;
						new_client.last_activity = time(NULL);
						new_client.state = ClientState::READING_REQ;
						_clients[pollfds[i].fd] = new_client;
					}

					ClientState& client = _clients[pollfds[i].fd];
					client.last_activity = time(NULL);

					char buffer[4096];
					ssize_t n;
					std::cout << "Reading from client fd: " << pollfds[i].fd << std::endl;
					n = recv(pollfds[i].fd, buffer, sizeof(buffer), 0);
					std::cout << "Read " << n << " bytes from client." << std::endl;

					if (n > 0)
					{
						// printf("========BUFFER:%s\n", buffer);
						client.request_buffer.append(buffer, n);
						// printf("========request:%s\n", client.request_buffer.c_str());
					}
					std::cout << "client request buffer: " << client.request_buffer << std::endl;
					//verifier si la requete est complete
					bool request_complete = false;
					size_t header_end = client.request_buffer.find("\r\n\r\n");
					if (header_end != std::string::npos)
					{
						std::cout << "Request headers complete." << std::endl;
						if (ft_check_body_size(pollfds[i].fd, client) == true)
						{
							//body size exceeded
							size_t server_index = _client_to_server[pollfds[i].fd];
							ServerConfig& server = _server[server_index];

							char poubelle[4096];
							ssize_t fd_reste_n;
							while ((fd_reste_n = recv(pollfds[i].fd, poubelle, sizeof(poubelle), MSG_DONTWAIT)) > 0)
							{
								// On ignore les données restantes
								// std::cout << "Discarding " << fd_reste_n << " bytes from client fd: " << pollfds[i].fd << std::endl;
							}

							_client_responses[pollfds[i].fd] = ft_handling_error(server, 413);
							pollfds[i].events = POLLOUT;
							client.request_buffer.clear();
							// request_complete = true;
							std::cout << "Body size exceeded during request parsing" << std::endl;

							// shutdown(pollfds[i].fd, SHUT_WR);
						}
						else
						{
							//gestion content-length
							size_t content_len_pos = client.request_buffer.find("Content-Length:");
							if (content_len_pos != std::string::npos)
							{
								size_t content_len_end = client.request_buffer.find("\r\n", content_len_pos);
								std::string cl_str = client.request_buffer.substr(content_len_pos + 15, content_len_end - (content_len_pos + 15));
								size_t content_length = std::atoi(cl_str.c_str());
								
								size_t total_expected = header_end + 4 + content_length;
								
								//lecture asynchrone- verifie juste si on a tout recu
								if (client.request_buffer.length() >= total_expected)
								{
									request_complete = true;
									client.request_buffer = client.request_buffer.substr(0, total_expected);
								}
								//sinon on attend le prochain poll() /  pas de while bloquant
							}
							else
							{
								request_complete = true;
							}
						}
					}
					else
					{
						//headers pas encore complets, on attend plus de données
					}

					if (n <= 0 && client.request_buffer.empty())
					{
						_clients.erase(pollfds[i].fd);
						close(pollfds[i].fd);
						pollfds.erase(pollfds.begin() + i);
						i--;
					}
					else if (request_complete)
					{
						std::cout << "Complete request received from client fd: " << pollfds[i].fd << std::endl;
						Response rep = parseRequest(client.request_buffer);

						size_t server_index = _client_to_server[pollfds[i].fd];
						ServerConfig& server = _server[server_index];
						Resultat resultat;
						std::string response;
						if (rep.invalid_request == 1)
						{
							resultat.setMessage(ft_handling_error(server, 400));
						}
						else
						{
							resultat.setMessage(Resultat::getRequest(rep, server, *this, _clients[pollfds[i].fd]));
							// resultat.getRequest(rep, server, *this, _clients[pollfds[i].fd]);
						}
						response = resultat.getMessage();
						// std::cout << response << std::endl;
						
						//reinitialiser le buffer pour les prochaines requetes keep-alive
						client.request_buffer.clear();
						
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
					}
					//sinon on attend plus de données, pas d'action, on revient au poll()
				} 

				if (pollfds[i].revents & POLLOUT)
				{
					std::string response_2 = _client_responses[pollfds[i].fd];
					std::cout << "============================================================" << std::endl;
					std::cout << "----RESPONSE-SENT-TO-CLIENT----" << std::endl;

					ClientState::State current_state = _clients[pollfds[i].fd].state;

					if (current_state == ClientState::WRITING_RES)
					{
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
					// size_t n = write(pollfds[i].fd, response_2.c_str(), response_2.length());
					size_t n = send(pollfds[i].fd, response_2.c_str(), response_2.length(), 0);
					std::cout << "Wrote " << n << " bytes to client fd: " << pollfds[i].fd << std::endl;

					std::cout << "----END-OF-RESPONSE-SENT-TO-CLIENT----" << std::endl;
					std::cout << "============================================================" << std::endl;
					
					_clients.erase(pollfds[i].fd);
					_client_responses.erase(pollfds[i].fd);
					_client_to_server.erase(pollfds[i].fd);
					
					std::cout << "Closing connection to client fd: " << pollfds[i].fd << std::endl;
					
					//clean

					close(pollfds[i].fd);
					pollfds.erase(pollfds.begin() + i);
					std::cout << "Connection closed for client fd: " << pollfds[i].fd << std::endl;

					i--;
					
					std::cout << "\n\n\n\n\n" << std::endl;
				}

				if (pollfds[i].revents & (POLLHUP | POLLERR))
				{
					std::cout << "POLLHUP or POLLERR detected on fd: " << pollfds[i].fd << ". Closing connection." << std::endl;
					close(pollfds[i].fd);
					pollfds.erase(pollfds.begin() + i);
					i--;
					// continue;
				}
			}
		}
	}
}