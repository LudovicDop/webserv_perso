/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_parsing.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hclaude <hclaude@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 00:46:16 by hclaude           #+#    #+#             */
/*   Updated: 2025/04/24 17:22:54 by hclaude          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int alloc_server(ServersDatas* data)
{
	try
	{
		data->server.push_back(new Server());
	}
	catch(const std::bad_alloc& e)
	{
		std::cerr << "Error: Memory allocation failed for server." << std::endl;
		return (1);
	}
	data->server_nb++;
	data->server[data->server_nb]->routes_nb = -1;
	data->server[data->server_nb]->client_max_body_size = -1;
	data->server[data->server_nb]->error_pages = 0;
	data->server[data->server_nb]->server_default = false;
	data->server[data->server_nb]->port = -1;
	return (0);
}

int	alloc_error_pages(Server* server)
{
	if (server->error_pages)
		return (1);
	try
	{
		server->error_pages = new(ErrorPages);
	}
	catch(const std::bad_alloc& e)
	{
		std::cerr << "Error: Memory allocation failed for error pages." << std::endl;
		return (1);
	}
	return (0);
}

int	alloc_route(Server* server)
{
	try
	{
		server->routes.push_back(new Route);
	}
	catch(const std::bad_alloc& e)
	{
		std::cerr << "Error: Memory allocation failed for route." << std::endl;
		return (1);
	}
	server->routes_nb++;
	server->routes[server->routes_nb]->cgi_enabled = false;
	server->routes[server->routes_nb]->cgi = 0;
	server->routes[server->routes_nb]->autoindex = false;
	return (0);
}

std::string	clear_line(const std::string&	buffer)
{
	int			i;
	std::string	final_line;

	i = 0;
	while (buffer[i])
	{
		while (buffer[i] == '\t' || buffer[i] == ' ')
			i++;

		if (buffer[i] == '#')
		{
			while (buffer[i] && buffer[i] != '\n')
			{
				i++;
			}
		}

		if (buffer[i] == '\n')
			i++;

		while (buffer[i] == '\t' || buffer[i] == ' ')
			i++;

		if (buffer[i] && buffer[i] != '\n' && buffer[i] != '\t' && buffer[i] != ' ' && buffer[i] != '#')
			final_line += buffer[i++];
	}
	return (final_line);
}

bool	syntax_check(std::string& line)
{
	int	i;
	int	bracket_nb;
	int	quote_nb;
	int	brace_nb;

	i = 0;
	bracket_nb = 0;
	quote_nb = 0;
	brace_nb = 0;
	while (line[i])
	{
		if (line[i] == '{')
			brace_nb++;
		if (line[i] == '}')
			brace_nb--;
		if (line[i] == '"')
			quote_nb++;
		if (line[i] == '[')
			bracket_nb++;
		if (line[i] == ']')
			bracket_nb--;
		i++;
	}
	if (brace_nb != 0 || quote_nb % 2 != 0 || bracket_nb != 0)
	{
		std::cerr << "Error: Mismatched braces, quotes, or brackets in syntax." << std::endl;
		return (false);
	}
	return (true);
}

bool	error_check(std::vector<Token> words)
{
	size_t	i = 0;

	while (i < words.size())
	{
		if (words[i].type == KEY && words[i + 1].type != COLON)
		{
			std::cerr << "Error: Expected ':' after key." << std::endl;
			return (false);
		}
		if (words[i].type == VALUE && words[i + 1].type != COMMA && words[i + 1].type != BRACE_CLOSE && words[i + 1].type != BRACKET_CLOSE)
		{
			std::cerr << "Error: Invalid syntax after value." << std::endl;
			return (false);
		}
		i++;
	}
	return (true);
}
