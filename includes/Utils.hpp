#pragma once

#include "Config.hpp"
#include "Response.hpp"
#include "webserv.hpp"

std::string& rtrim(std::string& s, const char* t);
std::string& ltrim(std::string& s, const char* t);
std::string& trim(std::string& s, const char* t);

void ft_free_double_tab(char **tab);
char *ft_strdup(const char *s1);
char	**ft_create_add_new_tab(char *str, char **tab, int size);
char **ft_add_double_tab(char *str, char **tab);
void ft_print_double_tab(char **tab);
std::string intToString(size_t n);
bool is_directory(const std::string &path);
std::string url_decode(const std::string &str);


