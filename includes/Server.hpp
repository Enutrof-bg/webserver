#pragma once

#include "Config.hpp"
#include "Response.hpp"
#include "webserv.hpp"

struct ServerConfig;
class Config;
struct ServerConfig;

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

public:
	Server(const Config &conf);
	~Server();

	bool is_listen_socket(int fd);
	void printListenPorts();
	void setup();
	void run();
	// void stop();
};