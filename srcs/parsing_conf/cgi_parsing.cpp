/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_parsing.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hclaude <hclaude@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 00:41:31 by hclaude           #+#    #+#             */
/*   Updated: 2025/04/24 18:15:48 by hclaude          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int	assign_enabled_cgi(size_t& i, std::vector<Token> words, Server* server)
{
	i+=2;
	if (server->routes[server->routes_nb]->cgi_enabled)
	{
		std::cerr << "Error: CGI 'enabled' directive can only be defined once." << std::endl;
		return (-1);
	}
	if (words[i].type != VALUE)
	{
		std::cerr << "Error: Invalid syntax for CGI 'enabled' directive. Expected a value." << std::endl;
		return (-1);
	}
	for (int j = 0; words[i].value[j]; j++)
	{
		words[i].value[j] = std::tolower(words[i].value[j]);
	}
	if (words[i].value == "true")
		server->routes[server->routes_nb]->cgi_enabled = true;
	else if (words[i].value == "false")
		server->routes[server->routes_nb]->cgi_enabled = false;
	else
	{
		std::cerr << "Error: Invalid value for CGI 'enabled' directive. Expected 'true' or 'false'." << std::endl;
		return (-1);
	}
	return (i);
}

int	assign_extensions_cgi(size_t& i, std::vector<Token> words, Server* server)
{
	int	brace_nb = 1;
	std::string value;
	std::string key;

	i+=2;
	if (!server->routes[server->routes_nb]->cgi->extensions.empty())
	{
		std::cerr << "Error: 'extensions' directive can only be defined once." << std::endl;
		return (-1);
	}
	if (words[i].type != BRACE_OPEN)
	{
		std::cerr << "Error: 'extensions' directive must start with an opening brace '{'." << std::endl;
		return (-1);
	}
	while (i < words.size() && brace_nb != 0)
	{
		if (words[i].value == "}")
			brace_nb--;
		if (words[i].type == COLON)
		{
			if (words[i + 1].type != VALUE)
			{
				std::cerr << "Error: 'extensions' directive requires a path value after the colon ':'." << std::endl;
				return (-1);
			}
			if (words[i - 1].type != KEY)
			{
				std::cerr << "Error: 'extensions' directive requires a key before the colon ':'." << std::endl;
				return (-1);
			}
			value = words[i + 1].value;
			key = words[i - 1].value;
			key.erase(std::remove(key.begin(), key.end(), '"'), key.end());
			value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
			server->routes[server->routes_nb]->cgi->extensions[key] = value;
		}
		i++;
	}
	return (i);
}

int	assign_cgi(size_t& i, std::vector<Token> words, Server* server)
{
	int	brace_nb = 1;
	int	err = 0;

	i+=2;
	if (server->routes[server->routes_nb]->cgi)
	{
		std::cerr << "Error: 'cgi' directive can only be defined once." << std::endl;
		return (-1);
	}
	if (words[i].type != BRACE_OPEN)
	{
		std::cerr << "Error: Invalid syntax for CGI directive. Expected an opening brace '{'." << std::endl;
		return (-1);
	}
	try
	{
		server->routes[server->routes_nb]->cgi = new CGI;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}
	i++;
	while (i < words.size() && brace_nb != 0 && err != -1)
	{
		if (words[i].value == "{")
			brace_nb++;
		if (words[i].value == "}")
			brace_nb--;
		if (words[i].type == KEY)
		{
			if (words[i].value == "enabled")
			{
				err = assign_enabled_cgi(i, words, server);
				continue;
			}
			else if (words[i].value == "extensions")
			{
				err = assign_extensions_cgi(i, words, server);
				continue;
			}
			else
			{
				std::cerr << "Error: Unknown key in CGI directive: '" << words[i].value << "'." << std::endl;
				return (-1);
			}
		}
		i++;
	}
	return (err);
}
