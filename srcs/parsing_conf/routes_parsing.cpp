/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routes_parsing.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hclaude <hclaude@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 00:45:22 by hclaude           #+#    #+#             */
/*   Updated: 2025/04/28 13:55:28 by hclaude          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int	assign_path(size_t& i, std::vector<Token> words, Server* server)
{
	i+=2;
	std::string value = words[i].value;

	if (!server->routes[server->routes_nb]->path.empty())
	{
		std::cerr << "Error: 'path' directive can only be defined once." << std::endl;
		return (-1);
	}
	if (words[i].type != VALUE)
	{
		std::cerr << "Error: Invalid syntax for 'path' directive. Expected a value." << std::endl;
		return (-1);
	}
	value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
	server->routes[server->routes_nb]->path = value;
	return (i);
}

int	assign_root(size_t& i, std::vector<Token> words, Server* server)
{
	i+=2;
	std::string value = words[i].value;

	if (!server->routes[server->routes_nb]->root.empty())
	{
		std::cerr << "Error: 'root' directive can only be defined once." << std::endl;
		return (-1);
	}
	if (words[i].type != VALUE)
	{
		std::cerr << "Error: Invalid syntax for 'root' directive. Expected a value." << std::endl;
		return (-1);
	}
	value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
	server->routes[server->routes_nb]->root = value;
	return (i);
}

int	assign_methods(size_t& i, std::vector<Token> words, Server* server)
{
	std::string value;

	int	bracket_nb = 0;
	i+=2;
	if (!server->routes[server->routes_nb]->methods.empty())
	{
		std::cerr << "Error: 'methods' directive can only be defined once." << std::endl;
		return (-1);
	}
	if (words[i++].type != BRACKET_OPEN)
	{
		std::cerr << "Error: Invalid syntax for 'methods' directive. Expected an opening bracket '['." << std::endl;
		return (-1);
	}
	bracket_nb++;
	while (i < words.size() && bracket_nb != 0)
	{
		if (words[i].type == BRACKET_CLOSE)
			bracket_nb--;
		else if (words[i].type == VALUE)
		{
			value = words[i].value;
			value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
			if (value != "GET" && value != "POST" && value != "DELETE")
			{
				std::cerr << "Error: Invalid method in 'methods' directive. Expected 'GET', 'POST', or 'DELETE'." << std::endl;
				return (-1);
			}
			server->routes[server->routes_nb]->methods.push_back(value);
		}
		i++;
	}
	if (bracket_nb != 0)
	{
		std::cerr << "Error: Mismatched brackets in 'methods' directive." << std::endl;
		return (-1);
	}
	return (i);
}

int	assign_index(size_t& i, std::vector<Token> words, Server* server)
{
	i+=2;
	std::string value = words[i].value;

	if (!server->routes[server->routes_nb]->index.empty())
	{
		std::cerr << "Error: 'index' directive can only be defined once." << std::endl;
		return (-1);
	}
	if (words[i].type != VALUE)
	{
		std::cerr << "Error: Invalid syntax for 'index' directive. Expected a value." << std::endl;
		return (-1);
	}
	value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
	server->routes[server->routes_nb]->index = value;
	return (i);
}

int	assign_autoindex(size_t& i, std::vector<Token> words, Server* server)
{
	i+=2;
	if (server->routes[server->routes_nb]->autoindex == true)
	{
		std::cerr << "Error: 'autoindex' directive can only be defined once." << std::endl;
		return (-1);
	}
	if (words[i].type != VALUE)
	{
		std::cerr << "Error: Invalid syntax for 'autoindex' directive. Expected a value." << std::endl;
		return (-1);
	}
	for (int j = 0; words[i].value[j]; j++)
	{
		words[i].value[j] = std::tolower(words[i].value[j]);
	}
	if (words[i].value == "true")
		server->routes[server->routes_nb]->autoindex = true;
	else if (words[i].value == "false")
		server->routes[server->routes_nb]->autoindex = false;
	else
	{
		std::cerr << "Error: Invalid value for 'autoindex' directive. Expected 'true' or 'false'." << std::endl;
		return (-1);
	}
	return (i);
}

int	assign_upload_dir(size_t& i, std::vector<Token> words, Server* server)
{
	i+=2;
	std::string value = words[i].value;

	if (!server->routes[server->routes_nb]->upload_dir.empty())
	{
		std::cerr << "Error: 'upload_dir' directive can only be defined once." << std::endl;
		return (-1);
	}
	if (words[i].type != VALUE)
	{
		std::cerr << "Error: Invalid syntax for 'upload_dir' directive. Expected a value." << std::endl;
		return (-1);
	}
	value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
	server->routes[server->routes_nb]->upload_dir = value;
	return (i);
}

int	assign_redirect(size_t& i, std::vector<Token> words, Server* server)
{
	i+=2;
	std::string value = words[i].value;

	if (!server->routes[server->routes_nb]->redirect.empty())
	{
		std::cerr << "Error: 'redirect' directive can only be defined once." << std::endl;
		return (-1);
	}
	if (words[i].type != VALUE)
	{
		std::cerr << "Error: Invalid syntax for 'redirect' directive. Expected a value." << std::endl;
		return (-1);
	}
	value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
	server->routes[server->routes_nb]->redirect = value;
	return (i);
}

int	assign_route(size_t& i, std::vector<Token> words, Server* server)
{
	int	brace_nb = 1;
	int	err = 0;

	i++;
	while (i < words.size() && brace_nb != 0 && err != -1)
	{
		if (words[i].type == BRACE_OPEN)
		{
			brace_nb++;
		}
		else if (words[i].type == BRACE_CLOSE)
		{
			brace_nb--;
		}
		else if (words[i].type == KEY)
		{
			if (words[i].value == "path")
			{
				err = assign_path(i, words, server);
				continue;
			}
			else if (words[i].value == "root")
			{
				err = assign_root(i, words, server);
				continue;
			}
			else if (words[i].value == "methods")
			{
				err = assign_methods(i, words, server);
				continue;
			}
			else if (words[i].value == "index")
			{
				err = assign_index(i, words, server);
				continue;
			}
			else if (words[i].value == "autoindex")
			{
				err = assign_autoindex(i, words, server);
				continue;
			}
			else if (words[i].value == "cgi")
			{
				err = assign_cgi(i, words, server);
				continue;
			}
			else if (words[i].value == "upload_dir")
			{
				err = assign_upload_dir(i, words, server);
				continue;
			}
			else if (words[i].value == "redirect")
			{
				err = assign_redirect(i, words, server);
				continue;
			}
			else
			{
				std::cerr << "Error: Unknown key in route directive: '" << words[i].value << "'." << std::endl;
				return (-1);
			}
		}
		i++;
	}
	return (err);
}

int	assign_routes(size_t& i, std::vector<Token> words, Server* server)
{
	int	bracket_nb = 1;
	int	err = 0;

	i += 2;
	if (!server->routes.empty())
	{
		std::cerr << "Error: 'routes' directive can only be defined once." << std::endl;
		return (-1);
	}
	if (words[i++].value != "[")
	{
		std::cerr << "Error: 'routes' directive must start with an opening bracket '['." << std::endl;
		return (-1);
	}
	while (i < words.size() && bracket_nb != 0)
	{
		if (words[i].value == "{")
		{
			if (alloc_route(server))
				return (1);
			err = assign_route(i, words, server);
			if (err == -1)
				return (-1);
			continue;
		}
		else if (words[i].value == "," && words[i + 1].value != "{")
		{
			std::cerr << "Error: Invalid syntax for 'routes' directive. Expected a value." << std::endl;
			return (-1);
		}
		else if (words[i].value == "[")
			bracket_nb++;
		else if (words[i].value == "]")
			bracket_nb--;
		i++;
	}
	if (bracket_nb != 0)
	{
		std::cerr << "Error: Mismatched brackets in 'routes' directive." << std::endl;
		return (-1);
	}
	return (i);
}
