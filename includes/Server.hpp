#pragma once

#include "Config.hpp"
#include "Response.hpp"
#include "Resultat.hpp"
#include "webserv.hpp"
#include "ClientState.hpp"

class ClientState;
class Config;
struct ServerConfig;
class Resultat;

#define TIMEOUT_SECONDS 3


class Server
{
private:
	std::vector<ServerConfig> _server;
	std::vector<sockaddr_in> _server_address;

	std::vector<int> _server_listen_socket;

	std::vector<int> _connfd;
	// std::map<int, int> _server_socket_to_server;

	std::vector<struct pollfd> _server_poll;
	// std::map<int, Client> _client;
	std::map<int, std::string> _client_responses;

	std::map<int, size_t> _client_to_server;

	std::vector<pollfd> pollfds;

	std::map<int, int> _cgi_pipe_client;

	int actual_port;

	std::map<int, ClientState> _clients;
	
public:
	Server(const Config &conf);
	~Server();

	bool is_listen_socket(int fd);
	void printListenPorts();
	void setup();
	void run();

	bool is_cgi_pipe_socket(int fd);
	bool is_cgi_pipe_socket_second(int fd);
	bool ft_is_fd_client_state(int fd);
	bool ft_is_timeout_over(int fd);
	void ft_check_timeout();
	void ft_remove_fd(int fd);
	bool ft_is_timeout(int fd);

	std::map<int, int> &get_cgi_pipe_client()
	{	return _cgi_pipe_client;
	}

	std::vector<pollfd> &get_pollfds()
	{	return pollfds;
	}

	int &get_actual_port()
	{	return actual_port;
	}	
	// void stop();
};

void ft_print_map(std::map<int, int> mp);