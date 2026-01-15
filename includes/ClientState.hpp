#pragma once
#include "webserv.hpp"

class ClientState
{
public:
	int fd_client;
	int fd_cgi;
	pid_t cgi_pid;
	std::string request_buffer;
	std::string response_buffer;

	time_t last_activity;

	// int state;
	enum State { IDLE, READING_REQ, WRITING_CGI, READING_CGI, WRITING_RES, TIMEOUT } state;

public:
	ClientState();
};
