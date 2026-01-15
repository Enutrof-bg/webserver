#pragma once

#include "ParseURL.hpp"
#include "Config.hpp"
#include "Response.hpp"
#include "Resultat.hpp"
#include "webserv.hpp"

class Config;
struct ServerConfig;
class ParseURL;
class ClientState;
class Server;
struct Location;
class Response;

class Resultat
{
private:

	std::string _message;
public:
	Resultat();

	Resultat(std::string message);

	std::string getMessage() const;

	void setMessage(std::string message);

	static std::string handleGET(const std::string &path, const ServerConfig &server, const Location &loc, const ParseURL &parsed_url);
	static std::string handlePOST(const Response &rep, const ServerConfig &server);
	static std::string handleDELETE(const Response &rep, const ServerConfig &server, Location &loc);
	static std::string handleCGI(const Response &rep, const ServerConfig &server, std::string path, const Location &loc, const ParseURL &parsed_url, Server &srv, ClientState &client_state);

	static std::string getRequest(Response &rep, const ServerConfig &server, Server &srv, ClientState &client_state);
// 	void getRequest(Response &rep, const ServerConfig &server, Server &srv, ClientState &client_state);
};