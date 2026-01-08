#pragma once

#include "Config.hpp"
#include "Response.hpp"
#include "webserv.hpp"

struct ServerConfig;
class Config;
struct ServerConfig;

// #define IDLE -1
// #define STATE_READING_REQUEST 0
// #define STATE_WRITING_RESPONSE 1
// #define STATE_CGI_PROCESSING 2

struct ClientState
{
	int fd_client;
	int fd_cgi;
	pid_t cgi_pid;
	std::string request_buffer;
	std::string response_buffer;

	time_t last_activity;

	// int state;
	enum State { IDLE, READING_REQ, WRITING_CGI, READING_CGI, WRITING_RES } state;

	ClientState(): fd_client(-1), fd_cgi(-1), cgi_pid(-1), request_buffer(""), response_buffer(""), state(IDLE) {}
};

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