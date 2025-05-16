/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hclaude <hclaude@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 00:45:22 by hclaude           #+#    #+#             */
/*   Updated: 2025/04/24 18:16:25 by hclaude          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int	assign_error_pages(size_t& i, std::vector<Token> words, Server* server)
{
	int	brace_nb = 1;
	std::string value;

	i+=2;
	if (words[i].type != BRACE_OPEN)
	{
		std::cerr << "Error: 'error_pages' directive must start with an opening brace '{'." << std::endl;
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
				std::cerr << "Error: 'error_pages' directive requires a path value after the colon ':'." << std::endl;
				return (-1);
			}
			if (words[i - 1].type != KEY)
			{
				std::cerr << "Error: 'error_pages' directive requires a key before the colon ':'." << std::endl;
				return (-1);
			}
			value = words[i + 1].value;
			value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
			server->error_pages->pages[words[i - 1].value] = value; // faut atoi je pense
		}
		i++;
	}
	return (i);
}

int	assign_server_names(size_t& i, std::vector<Token> words, Server* server)
{
	int	bracket_nb = 1;
	std::string value;

	i++;
	if (!server->server_names.empty())
	{
		std::cerr << "Error: 'server_names' directive can only be defined once." << std::endl;
		return (-1);
	}
	if (words[++i].value != "[")
	{
		std::cerr << "Error: 'server_names' directive must start with an opening bracket '['." << std::endl;
		return (-1);
	}
	while (++i < words.size() && bracket_nb != 0)
	{
		if (words[i].value == "]")
			bracket_nb--;
		else if (words[i].type == VALUE)
		{
			value = words[i].value;
			value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
			server->server_names.push_back(value);
		}
	}
	if (bracket_nb != 0)
	{
		std::cerr << "Error: Missing closing bracket ']' in 'server_names' directive." << std::endl;
		return (-1);
	}
	return (i);
}

int	assign_other_things(size_t& i, std::vector<Token> words, Server* server)
{
	std::string value;

	if (words[i].value == "host")
	{
		i+=2;
		if (!server->host.empty())
		{
			std::cerr << "Error: 'host' directive can only be defined once." << std::endl;
			return (-1);
		}
		if (words[i].type != VALUE)
		{
			std::cerr << "Error: 'host' directive requires a value." << std::endl;
			return (-1);
		}
		value = words[i].value;
		value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
		server->host = value;
	}
	else if (words[i].value == "port")
	{
		i+=2;
		if (server->port != -1)
		{
			std::cerr << "Error: 'port' directive can only be defined once." << std::endl;
			return (-1);
		}
		if (words[i].type != VALUE)
		{
			std::cerr << "Error: 'port' directive requires a numeric value." << std::endl;
			return (-1);
		}
		server->port = atoi(words[i].value.c_str());
		if (server->port < 1 || server->port > 65535)
		{
			std::cerr << "Error: Port number must be between 1 and 65535." << std::endl;
			return (-1);
		}
	}
	else if (words[i].value == "default")
	{
		i+=2;
		if (server->server_default == true)
		{
			std::cerr << "Error: 'default' directive can only be defined once." << std::endl;
			return (-1);
		}
		if (words[i].type != VALUE)
		{
			std::cerr << "Error: 'default' directive requires a boolean value ('true' or 'false')." << std::endl;
			return (-1);
		}
		for (int j = 0; words[i].value[j]; j++)
		{
			words[i].value[j] = std::tolower(words[i].value[j]);
		}
		if (words[i].value == "true")
			server->server_default = true;
		else if (words[i].value == "false")
			server->server_default = false;
		else
		{
			std::cerr << "Error: The value for 'default' must be 'true' or 'false'" << std::endl;
			return (-1);
		}
	}
	else if (words[i].value == "client_max_body_size")
	{
		i+=2;
		if (server->client_max_body_size != -1)
		{
			std::cerr << "Error: 'client_max_body_size' directive can only be defined once." << std::endl;
			return (-1);
		}
		if (words[i].type != VALUE)
		{
			std::cerr << "Error: 'client_max_body_size' directive requires a numeric value." << std::endl;
			return (-1);
		}
		server->client_max_body_size = atoi(words[i].value.c_str());
		if (server->client_max_body_size <= 0 || server->client_max_body_size > 1073741824)
		{
			std::cerr << "Error: 'client_max_body_size' must be a positive value and not exceeds maximum allowed value (1073741824)." << std::endl;
			return (-1);
		}
	}
	else
	{
		std::cerr << "Error: Unknown directive: '" << words[i].value << "'." << std::endl;
		return (-1);
	}
	return (1);
}

int	parse_this_shit(std::vector<Token> words, ServersDatas* data)
{
	int	err = 0;

	for (size_t i = 0; i < words.size(); i++)
	{
		if (words[i].value == "server")
		{
			if (alloc_server(data))
				return (1);
		}
		else if (data->server_nb == -1)
		{
			std::cerr << "Error: No server block defined." << std::endl;
			return (1);
		}
		else if (words[i].value == "error_pages")
		{
			if (alloc_error_pages(data->server[data->server_nb]))
			{
				std::cerr << "Error: Failed to allocate error pages." << std::endl;
				return (1);
			}
			err = assign_error_pages(i, words, data->server[data->server_nb]);
		}
		else if (words[i].value == "routes")
		{
			err = assign_routes(i, words, data->server[data->server_nb]);
		}
		else if (words[i].value == "server_names")
		{
			err = assign_server_names(i, words, data->server[data->server_nb]);
		}
		else if (words[i].type == KEY)
		{
			err = assign_other_things(i, words, data->server[data->server_nb]);
		}
		if (err == -1)
			return (1);
	}
	return (0);
}
