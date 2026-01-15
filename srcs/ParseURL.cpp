#include "../includes/ParseURL.hpp"
#include "../includes/Response.hpp"

//Parse l'url pour trouver le path_script, path_info et query_string
//Utilise les infos de server pour trouver les extensions cgi
//Retourne une structure ParseURL
ParseURL ParseURL::ft_parseURL(const Response &rep, const ServerConfig &server)
{
	(void)rep;
	(void)server;
	ParseURL result;
	std::string url = rep.url;
	size_t query_pos = url.find('?');
	if (query_pos != std::string::npos)
	{
		result.url = url.substr(0, query_pos);
		result.query_string = url.substr(query_pos + 1);
	}
	else
	{
		result.url = url;
		result.query_string = "";  // Vide si pas de ? dans l'URL
	}
	
	//check tout les location pour trouver les extensions CGI
	for (size_t j = 0; j < server._config_location.size(); ++j)
	{
		std::vector<std::string> cgi_exts = server._config_location[j]._config_cgi_ext;
		if (cgi_exts.empty())
			continue;
		
		//parcours des extensions CGI pour cette location
		for (size_t i = 0; i < cgi_exts.size(); ++i)
		{
			std::string ext = cgi_exts[i];
			size_t ext_pos = result.url.find(ext);
			
			if (ext_pos != std::string::npos)
			{
				size_t script_end = ext_pos + ext.length();
				result.path_script = result.url.substr(0, script_end);
				

				if (script_end < result.url.length())
					result.path_info = result.url.substr(script_end);
				else
					result.path_info = "";
				
				return result;
			}
		}
	}
	// aucun cgi trouve
	result.path_script = result.url;
	result.path_info = "";

	return result;
}
