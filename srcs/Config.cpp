#include "../includes/Config.hpp"

Config::Config(std::string filename): _filename(filename)
{
}
Config::~Config()
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
std::vector<ServerConfig> Config::getServer() const
{
	return _server;
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

inline std::string& rtrim(std::string& s, const char* t)
{
	size_t pos = s.find_last_not_of(t);
	if (pos != std::string::npos)
		s.erase(pos + 1);
	else
		s.clear();
    return s;
}

inline std::string& ltrim(std::string& s, const char* t)
{
	size_t pos = s.find_first_not_of(t);
	if (pos != std::string::npos)
		s.erase(0, pos);
	else
		s.clear();
    return s;
}

inline std::string& trim(std::string& s, const char* t)
{
    return ltrim(rtrim(s, t), t);
}

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

void Config::parseLocation(std::istringstream &str, Location &location)
{
	std::string token;
	// std::cout << "ENtry" << std::endl;
	while (str >> token)
	{
		// std::cout << "ENtry2" << std::endl;
		std::cout << "[" << token << "]";
		if (token == "}")
		{
			// std::cout << "ENtry3" << std::endl;
			break;
		}
		else if (token == "autoindex")
		{
			std::cout << "ENtry4" << std::endl;
			std::string temp;
			str >> temp;
			// std::cout << temp << std::endl;
			location._config_autoindex = (temp == "on;");
			std::cout << location._config_autoindex << std::endl;
		}
		else if (token == "allowed_methods" || token == "allow_methods")
		{
			std::cout << "ENtry5" << std::endl;
			std::string method;
			//allowed_method a refaire, ne fonctionne pas correctement
			getline(str, method);
			std::cout << "METHOD:" <<method << std::endl;

			std::stringstream ss(method);
			std::string temp;

			while (ss >> temp)
			{
				location._config_allowed_methods.push_back(temp);
				std::cout << "TEMP:"<< temp << std::endl;
			}
            // while (str >> method)
			// {
            //     if (method == "}" || method == "autoindex" || method == "upload_path" || 
            //         method == "cgi_pass" || method == "return")
			// 	{
            //         str.putback(' ');
            //         for (int i = method.length() - 1; i >= 0; --i)
			// 		{
            //             str.putback(method[i]);
            //         }
            //         break;
            //     }
            //     location._config_allowed_methods.push_back(method);
			// 	std::cout << method << std::endl;
			// }
			// std::cout << str << std::endl;
		}
	}
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
		std::cout << "[" <<token << "]";
		if (token == "}")
		{
			break;
		}
		else if (token == "listen")
		{
			str >> server._config_listen;
			// server._config_listen = rtrim(server._config_listen, " \t");
			// server._config_listen = rtrim(server._config_listen, ";");
			std::cout << server._config_listen << std::endl;
		}
		else if (token == "server_name")
		{
			str >> server._config_server_name;
			server._config_server_name = rtrim(server._config_server_name, " \t");
			server._config_server_name = rtrim(server._config_server_name, ";");
			std::cout << server._config_server_name << std::endl;
		}
		else if (token == "root")
		{
			str >> server._config_root;
			server._config_root = rtrim(server._config_root, " \t");
			server._config_root = rtrim(server._config_root, ";");
			std::cout << "{" <<server._config_root << "}"<<std::endl;
		}
		else if (token == "index")
		{
			str >> server._config_index;
			server._config_index = rtrim(server._config_index, " \t");
			server._config_index = rtrim(server._config_index, ";");
			std::cout << server._config_index << std::endl;
		}
		else if (token == "client_max_body_size")
		{
			str >> server._config_client_max_body_size;
			// server._config_root = rtrim(server._config_root, " \t");
			// server._config_root = rtrim(server._config_root, ";");
			std::cout << server._config_client_max_body_size << std::endl;
		}
		else if (token == "error_page")
		{
			int code;
			std::string page;
			str >> code;
			str >> page;
			page = rtrim(page, " \t");
			page = rtrim(page, ";");
			server._config_error_page[code] = page;
			std::cout << code << ":" << server._config_error_page[code] << std::endl;
		}
		else if (token == "location")
		{
			std::string path;
			str >> path >> token;
			// std::cout << "path:" <<path<< std::endl;
			// std::cout << "token:" << token << std::endl;
			if (token == "{")
			{
				Location location;
				location._config_path = path;
				location._config_autoindex = false;
				parseLocation(str, location);
				server._config_location.push_back(location);
			}
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


void Config::printConfig() const
{
	std::vector<ServerConfig>::iterator it;
	// for (it = _server.begin(); it != _server.end(); it++)
	//  for (size_t i = 0; i < this->_server.size(); ++i)
	// {
	// }
    for (size_t i = 0; i < this->_server.size(); ++i)
	{
        const ServerConfig& server = this->_server[i];
        std::cout << "Server " << i + 1 << ":" << std::endl;
        std::cout << "  Port: " << server._config_listen << std::endl;
        std::cout << "  Server name: " << server._config_server_name << std::endl;
        std::cout << "  Root: " << server._config_root << std::endl;
        std::cout << "  Index: " << server._config_index << std::endl;
        std::cout << "  Max body size: " << server._config_client_max_body_size << std::endl;
        
        for (size_t j = 0; j < server._config_location.size(); ++j)
		{
            const Location& loc = server._config_location[j];
            std::cout << "  Location " << loc._config_path << ":" << std::endl;
            std::cout << "    Methods: ";
            for (size_t k = 0; k < loc._config_allowed_methods.size(); ++k)
			{
                std::cout << loc._config_allowed_methods[k] << " ";
            }
            std::cout << std::endl;
            std::cout << "    Autoindex: " << (loc._config_autoindex ? "on" : "off") << std::endl;
        }
        std::cout << std::endl;
    }
}