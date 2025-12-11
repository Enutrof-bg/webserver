#include "../includes/Config.hpp"

Config::Config(std::string filename): _filename(filename)
{
}

std::string Config::getFilename() const
{
	return _filename;
}
std::string Config::getConfig() const
{
	return _config;
}
// std::string Config::getConfig_listen() const
// {
// 	return _config_listen;
// }
// std::string Config::getConfig_server_name() const
// {
// 	return _config_server_name;
// }
// std::string Config::getConfig_name() const
// {
// 	return _config_name;
// }
// std::string Config::getConfig_index() const
// {
// 	return _config_index;
// }
// std::string Config::getConfig_client_max() const
// {
// 	return _config_client_max;
// }
// std::string Config::getConfig_error_page() const
// {
// 	return _config_error_page;
// }
// std::string Config::getConfig_autoindex() const
// {
// 	return _config_autoindex;
// }
// std::string Config::getConfig_allowed_methods() const
// {
// 	return _config_allowed_methods;
// }
// std::string Config::getConfig_upload_path() const
// {
// 	return _config_upload_path;
// }
// std::string Config::getConfig_return() const
// {
// 	return _config_return;
// }
// std::string Config::getConfig_cgi_pass() const
// {
// 	return _config_cgi_pass;
// }

void Config::setConfig(std::string &config)
{
	_config = config;
}
// void Config::setConfig_listen(std::string &config)
// {
// 	_config_listen = config;
// }
// void Config::setConfig_server_name(std::string &config)
// {
// 	_config_server_name = config;
// }
// void Config::setConfig_name(std::string &config)
// {
// 	_config_name = config;
// }
// void Config::setConfig_index(std::string &config)
// {
// 	_config_index = config;
// }
// void Config::setConfig_client_max(std::string &config)
// {
// 	_config_client_max = config;
// }
// void Config::setConfig_error_page(std::string &config)
// {
// 	_config_error_page = config;
// }
// void Config::setConfig_autoindex(std::string &config)
// {
// 	_config_autoindex = config;
// }
// void Config::setConfig_allowed_methods(std::string &config)
// {
// 	_config_allowed_methods = config;
// }
// void Config::setConfig_upload_path(std::string &config)
// {
// 	_config_upload_path = config;
// }
// void Config::setConfig_return(std::string &config)
// {
// 	_config_return = config;
// }
// void Config::setConfig_cgi_pass(std::string &config)
// {
// 	_config_cgi_pass = config;
// }

std::string Config::readConfig()
{
	std::ifstream infile;
	infile.open(getFilename().c_str());
	if (!infile)
	{
		throw std::runtime_error("Error: configuration file open failed");
	}

	std::string line;
	std::string content;

	while (std::getline(infile, line))
	{
		content += line + "\n";
	}
	this->setConfig(content);

	infile.close();
	return (_config);
}
void Config::parseServer(std::istringstream &str, ServerConfig &server)
{
	server._config_listen = 80;
	server._config_server_name = "localhost";
	server._config_root = "./html";
	server._config_index = "index.html";
	server._config_client_max_body_size = 1048576;

	std::string token;
	while (str >> token)
	{
		// std::cout << token << std::endl;
		if (token == "}")
		{
			break;
		}
		else if (token == "listen")
		{
			str >> server._config_listen;
			std::cout << server._config_listen << std::endl;
		}
		else if (token == "server_name")
		{
			str >> server._config_server_name;
			std::cout << server._config_server_name << std::endl;
		}
		else if (token == "root")
		{
			str >> server._config_root;
			std::cout << server._config_root << std::endl;
		}
		else if (token == "index")
		{
			str >> server._config_index;
			std::cout << server._config_index << std::endl;
		}
		else if (token == "client_max_body_size")
		{
			str >> server._config_client_max_body_size;
			std::cout << server._config_client_max_body_size << std::endl;
		}
		else if (token == "error_page")
		{
			int code;
			std::string page;
			str >> code;
			str >> page;
			server._config_error_page[code] = page;
			std::cout << code << ":" << server._config_error_page[code] << std::endl;
		}
	}
}

void Config::parseConfig()
{
	std::cout << "TEST" << std::endl;
	std::istringstream myConfig(this->getConfig());
	std::string token;

	while (myConfig >> token)
	{
		// std::cout << token << std::endl;
		if (token == "server")
		{
			myConfig >> token;
			if (token == "{")
			{
				ServerConfig server;
				parseServer(myConfig, server);
				_server.push_back(server);
			}
		}
	}
}