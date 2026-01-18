#include "../includes/Utils.hpp"

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

void ft_free_double_tab(char **tab)
{
	int		i;

	i = 0;
	if (!tab)
		return ;
	while (tab[i])
	{
		delete[] tab[i];
		tab[i] = NULL;
		i++;
	}
	delete[] tab;
	tab = NULL;
}

char *ft_strdup(const char *s1)
{
	char	*dup;
	size_t	i;

	dup = new char[strlen(s1) + 1];
	if (!dup)
		return (NULL);
	i = 0;
	while (s1[i])
	{
		dup[i] = s1[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

char	**ft_create_add_new_tab(char *str, char **tab, int size)
{
	int		i;
	char	**newtab;

	i = 0;
	newtab = new char*[size + 2];
	if (!newtab)
		return (NULL);
	while (tab[i])
	{
		newtab[i] = ft_strdup(tab[i]);
		if (!newtab[i])
		{
			ft_free_double_tab(newtab);
			return NULL;
		}
		i++;
	}
	newtab[i] = ft_strdup(str);
	if (!newtab[i])
	{
		ft_free_double_tab(newtab);
		return NULL;
	}
	i++;
	newtab[i] = 0;
	return (newtab);
}
//from minishell :)
char	**ft_add_double_tab(char *str, char **tab)
{
	int		i;
	char	**newtab;

	i = 0;
	if (!str)
		return (tab);
	if (tab == NULL)
	{
		newtab = new char*[2];
		if (!newtab)
			return (NULL);
		newtab[0] = ft_strdup(str);
		if (!newtab[0])
		{
			delete[] newtab;
			newtab = NULL;
			return NULL;
		}
		return (newtab[1] = 0, newtab);
	}
	while (tab[i])
		i++;
	newtab = ft_create_add_new_tab(str, tab, i);
	return (ft_free_double_tab(tab), newtab);
}

void ft_print_double_tab(char **tab)
{
	int		i;

	i = 0;
	if (!tab)
	{
		std::cout << "tab is NULL" << std::endl;
		return ;
	}
	while (tab[i])
	{
		std::cout << "tab[" << i << "]:" << "{" << tab[i] << "}" << std::endl;
		i++;
	}
}

std::string intToString(size_t n)
{
	std::ostringstream oss;
	oss << n;
	return oss.str();
}

//Check if a path is a directory(true) or not(false)
bool is_directory(const std::string &path)
{
    DIR *dir = opendir(path.c_str());
    if (dir != NULL) {
        closedir(dir);
		return true;
    }
    return false;
}

//Decode URL encoded string (%XX -> character)
std::string url_decode(const std::string &str)
{
	std::string result;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == '%' && i + 2 < str.length())
		{
			int hex_val;
			std::istringstream hex_stream(str.substr(i + 1, 2));
			if (hex_stream >> std::hex >> hex_val)
			{
				result += static_cast<char>(hex_val);
				i += 2;
			}
			else
				result += str[i];
		}
		else if (str[i] == '+')
			result += ' ';
		else
			result += str[i];
	}
	return result;
}