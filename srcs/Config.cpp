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

std::string& rtrim(std::string& s, const char* t)
{
	size_t pos = s.find_last_not_of(t);
	if (pos != std::string::npos)
		s.erase(pos + 1);
	else
		s.clear();
	return s;
}

std::string& ltrim(std::string& s, const char* t)
{
	size_t pos = s.find_first_not_of(t);
	if (pos != std::string::npos)
		s.erase(0, pos);
	else
		s.clear();
	return s;
}

std::string& trim(std::string& s, const char* t)
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
	
	location._config_autoindex = false;
	location._config_client_max_body_size = 0;
	
	while (str >> token)
	{
		std::cout << "[LOCATION TOKEN: " << token << "]" << std::endl;
		
		if (token == "}")
		{
			break;
		}
		else if (token == "autoindex")
		{
			std::string temp;
			str >> temp;
			temp = rtrim(temp, " \t;");
			location._config_autoindex = (temp == "on");
			std::cout << "Autoindex: " << location._config_autoindex << std::endl;
		}
		else if (token == "allowed_methods" || token == "allow_methods")
		{
			std::string method;
			getline(str, method);
			method = rtrim(method, " \t;");
			
			std::stringstream ss(method);
			std::string temp;
			
			while (ss >> temp)
			{
				temp = rtrim(temp, " \t;");
				if (!temp.empty())
				{
					location._config_allowed_methods.push_back(temp);
					std::cout << "Method: " << temp << std::endl;
				}
			}
		}
		else if (token == "root")
		{
			str >> location._config_root;
			location._config_root = rtrim(location._config_root, " \t;");
			std::cout << "Root: " << location._config_root << std::endl;
		}
		else if (token == "index")
		{
			str >> location._config_index;
			location._config_index = rtrim(location._config_index, " \t;");
			std::cout << "Index: " << location._config_index << std::endl;
		}
		else if (token == "client_max_body_size")
		{
			std::string body_size;
			str >> body_size;
			body_size = rtrim(body_size, " \t;");
			
			size_t len = body_size.length();
			if (len > 0)
			{
				char unit = body_size[len - 1];
				long multiplier = 1;
				std::string num_part = body_size;
				
				if (unit == 'M' || unit == 'm')
				{
					multiplier = 1024 * 1024;
					num_part = body_size.substr(0, len - 1);
				}
				else if (unit == 'K' || unit == 'k')
				{
					multiplier = 1024;
					num_part = body_size.substr(0, len - 1);
				}
				else if (unit == 'G' || unit == 'g')
				{
					multiplier = 1024 * 1024 * 1024;
					num_part = body_size.substr(0, len - 1);
				}
				
				std::istringstream iss(num_part);
				long value;
				if (iss >> value)
				{
					location._config_client_max_body_size = value * multiplier;
					std::cout << "client_max_body_size: " << location._config_client_max_body_size << " bytes" << std::endl;
				}
			}
		}
		else if (token == "cgi_path")
		{
			std::string path;
			getline(str, path);
			path = rtrim(path, " \t;");
			
			std::stringstream ss(path);
			std::string temp;
			
			while (ss >> temp)
			{
				temp = rtrim(temp, " \t;");
				if (!temp.empty())
				{
					// location._config_cgi_path.push_back(temp);
					location._config_cgi_path.append(temp);
					std::cout << "CGI Path: " << temp << std::endl;
				}
			}
		}
		else if (token == "cgi_ext")
		{
			std::string ext;
			getline(str, ext);
			ext = rtrim(ext, " \t;");
			
			std::stringstream ss(ext);
			std::string temp;
			
			while (ss >> temp)
			{
				temp = rtrim(temp, " \t;");
				if (!temp.empty())
				{
					location._config_cgi_ext.push_back(temp);
					std::cout << "CGI Extension: " << temp << std::endl;
				}
			}
		}
		else if (token == "return")
		{
			str >> location._config_redirect;
			location._config_redirect = rtrim(location._config_redirect, " \t;");
			std::cout << "Redirect: " << location._config_redirect << std::endl;
		}
		else if (token == "upload_path")
		{
			str >> location._config_upload_path;
			location._config_upload_path = rtrim(location._config_upload_path, " \t;");
			std::cout << "Upload path: " << location._config_upload_path << std::endl;
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
			std::string body_size;
			str >> body_size;
			body_size = rtrim(body_size, " \t;");
			
			size_t len = body_size.length();
			if (len > 0)
			{
				char unit = body_size[len - 1];
				long multiplier = 1;
				std::string num_part = body_size;
				
				if (unit == 'M' || unit == 'm')
				{
					multiplier = 1024 * 1024;
					num_part = body_size.substr(0, len - 1);
				}
				else if (unit == 'K' || unit == 'k')
				{
					multiplier = 1024;
					num_part = body_size.substr(0, len - 1);
				}
				else if (unit == 'G' || unit == 'g')
				{
					multiplier = 1024 * 1024 * 1024;
					num_part = body_size.substr(0, len - 1);
				}
				
				std::istringstream iss(num_part);
				long value;
				if (iss >> value)
				{
					server._config_client_max_body_size = value * multiplier;
					std::cout << "client_max_body_size: " << server._config_client_max_body_size << " bytes" << std::endl;
				}
			}
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
			std::cout << "path:" <<path<< std::endl;
			std::cout << "token:" << token << std::endl;
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
	for (size_t i = 0; i < this->_server.size(); ++i)
	{
		const ServerConfig& server = this->_server[i];
		std::cout << "\n===Server" << i + 1 << "===" << std::endl;
		std::cout << "Port: " << server._config_listen << std::endl;
		std::cout << "Server name: " << server._config_server_name << std::endl;
		std::cout << "Root: " << server._config_root << std::endl;
		std::cout << "Index: " << server._config_index << std::endl;
		std::cout << "Max body size: " << server._config_client_max_body_size << " bytes" << std::endl;
		
		if (!server._config_error_page.empty())
		{
			std::cout << "Error pages:" << std::endl;
			std::map<int, std::string>::const_iterator it;
			for (it = server._config_error_page.begin(); it != server._config_error_page.end(); ++it)
			{
				std::cout << "	" << it->first << " = " << it->second << std::endl;
			}
		}
		

		for (size_t j = 0; j < server._config_location.size(); ++j)
		{
			const Location& loc = server._config_location[j];
			std::cout << "\n---Location: " << loc._config_path << "---" << std::endl;
			
			if (!loc._config_allowed_methods.empty())
			{
				std::cout << "Allowed methods: ";
				for (size_t k = 0; k < loc._config_allowed_methods.size(); ++k)
				{
					std::cout << loc._config_allowed_methods[k];
					if (k < loc._config_allowed_methods.size() - 1)
						std::cout << ", ";
				}
				std::cout << std::endl;
			}
			
			std::cout << "Autoindex: " << (loc._config_autoindex ? "on" : "off") << std::endl;
			
			if (!loc._config_root.empty())
				std::cout << "Root: " << loc._config_root << std::endl;
			
			if (!loc._config_index.empty())
				std::cout << "Index: " << loc._config_index << std::endl;
			
			if (loc._config_client_max_body_size > 0)
				std::cout << "Max body size: " << loc._config_client_max_body_size << " bytes" << std::endl;
			
			if (!loc._config_cgi_path.empty())
			{
				std::cout << "CGI paths: " << loc._config_cgi_path << std::endl;
				// std::cout << "CGI paths: ";
				// for (size_t k = 0; k < loc._config_cgi_path.size(); ++k)
				// {
				//	 std::cout << loc._config_cgi_path[k];
				//	 if (k < loc._config_cgi_path.size() - 1)
				//		 std::cout << ", ";
				// }
				std::cout << std::endl;
			}
			
			if (!loc._config_cgi_ext.empty())
			{
				std::cout << "CGI extensions: ";
				for (size_t k = 0; k < loc._config_cgi_ext.size(); ++k)
				{
					std::cout << loc._config_cgi_ext[k];
					if (k < loc._config_cgi_ext.size() - 1)
						std::cout << ", ";
				}
				std::cout << std::endl;
			}
			
			if (!loc._config_redirect.empty())
				std::cout << "Redirect: " << loc._config_redirect << std::endl;
			
			if (!loc._config_upload_path.empty())
				std::cout << "Upload path: " << loc._config_upload_path << std::endl;
		}
		std::cout << std::endl;
	}
}