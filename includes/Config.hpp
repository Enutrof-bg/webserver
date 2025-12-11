#pragma once

#include "webserv.hpp"

class Config
{
private:
	std::string _filename;
	std::string _config;

	//config info obligatoire
	std::string _config_listen;
	std::string _config_server_name;
	std::string _config_name;

	//confog info optionnelle
	std::string _config_index;
	std::string _config_client_max;
	std::string _config_error_page;
	std::string _config_autoindex;
	std::string _config_allowed_methods;
	std::string _config_upload_path;
	std::string _config_return;
	std::string _config_cgi_pass;

public:
	//Constructor
	Config(std::string filename);

	//Getter
	std::string getFilename() const;
	std::string getConfig() const;

	std::string getConfig_listen() const;
	std::string getConfig_server_name() const;
	std::string getConfig_name() const;
	std::string getConfig_index() const;
	std::string getConfig_client_max() const;
	std::string getConfig_error_page() const;
	std::string getConfig_autoindex() const;
	std::string getConfig_allowed_methods() const;
	std::string getConfig_upload_path() const;
	std::string getConfig_return() const;
	std::string getConfig_cgi_pass() const;

	//Setter
	void setConfig(std::string &config);
	void setConfig_listen(std::string &config);
	void setConfig_server_name(std::string &config);
	void setConfig_name(std::string &config);
	void setConfig_index(std::string &config);
	void setConfig_client_max(std::string &config);
	void setConfig_error_page(std::string &config);
	void setConfig_autoindex(std::string &config);
	void setConfig_allowed_methods(std::string &config);
	void setConfig_upload_path(std::string &config);
	void setConfig_return(std::string &config);
	void setConfig_cgi_pass(std::string &config);

	//Truc
	std::string readConfig();

	//validSyntax()

	//tokenize()

	//parseServer()

	void parseConfig();
};