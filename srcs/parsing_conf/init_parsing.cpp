/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_parsing.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hclaude <hclaude@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 18:30:18 by hclaude           #+#    #+#             */
/*   Updated: 2025/04/28 20:57:36 by hclaude          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

ServersDatas *ServersDatas::getInstance()
{
	if (!_instance)
		_instance = new ServersDatas();
	return _instance;
}

ServersDatas *ServersDatas::_instance = NULL;

ServersDatas::ServersDatas() : server_nb(-1) {}

int	read_and_get(std::ifstream& fileStream, ServersDatas* data)
{
	std::string					line;
	std::vector<Token>			words_token;

	std::stringstream	buffer;
	buffer << fileStream.rdbuf();
	line = clear_line(buffer.str());
	if (line.empty())
	{
		std::cerr << "Error: file empty." << std::endl;
		return (1);
	}
	if (!syntax_check(line))
		return (1);
	words_token = tokenize(line);
	if (!error_check(words_token))
		return (1);
	if (parse_this_shit(words_token, data))
		return (1);
	return (0);
}

int	get_datas(const std::string& file_path, ServersDatas* data)
{
	std::ifstream	fileStream(file_path.c_str());

	if (!fileStream.is_open())
	{
		std::cerr << "Error: Failed to open configuration file. " << std::string(strerror(errno)) << std::endl;
		return (1);
	}
	return (read_and_get(fileStream, data));
}

void print_server_datas(const ServersDatas& data)
{
	data.printConfig();
	if (data.server_nb < 0 || data.server.empty())
	{
		std::cerr << "Error: No server data available." << std::endl;
		return;
	}

	std::cout << "\n\033[1;36m╔════════════════════════════════════════════════════════╗\033[0m" << std::endl;
	std::cout << "\033[1;36m║  CONFIGURATION DES SERVEURS (" << data.server_nb + 1 << " serveurs)               ║\033[0m" << std::endl;
	std::cout << "\033[1;36m╚════════════════════════════════════════════════════════╝\033[0m\n" << std::endl;

	for (int i = 0; i <= data.server_nb && i < static_cast<int>(data.server.size()); i++)
	{
		if (!data.server[i])
		{
			std::cerr << "Error: Null server pointer at index " << i << "." << std::endl;
			continue;
		}

		Server* server = data.server[i];

		std::cout << "\033[1;33m┌───────────────────────────────────────┐\033[0m" << std::endl;
		std::cout << "\033[1;33m│            SERVEUR " << (i + 1) << "                  │\033[0m" << std::endl;
		std::cout << "\033[1;33m├───────────────────────────────────────┤\033[0m" << std::endl;
		std::cout << "\033[1;33m│\033[0m \033[1mHost:\033[0m            " << server->host << std::endl;
		std::cout << "\033[1;33m│\033[0m \033[1mPort:\033[0m            " << server->port << std::endl;

		std::cout << "\033[1;33m│\033[0m \033[1mNoms de serveur:\033[0m ";
		if (server->server_names.empty())
		{
			std::cout << "aucun" << std::endl;
		}
		else
		{
			for (std::vector<std::string>::const_iterator it = server->server_names.begin(); it != server->server_names.end(); ++it)
			{
				std::cout << *it;
				if (it + 1 != server->server_names.end())
					std::cout << ", ";
			}
			std::cout << std::endl;
		}

		std::cout << "\033[1;33m│\033[0m \033[1mServeur défaut:\033[0m  " << (server->server_default ? "oui" : "non") << std::endl;
		std::cout << "\033[1;33m│\033[0m \033[1mMax body size:\033[0m   " << server->client_max_body_size << std::endl;

		// Pages d'erreur
		std::cout << "\033[1;33m│\033[0m" << std::endl;
		std::cout << "\033[1;33m│\033[0m \033[1;35m┌─── PAGES D'ERREUR ───┐\033[0m" << std::endl;
		if (server->error_pages && !server->error_pages->pages.empty())
		{
			for (std::map<std::string, std::string>::const_iterator it = server->error_pages->pages.begin(); it != server->error_pages->pages.end(); ++it)
			{
				std::cout << "\033[1;33m│\033[0m \033[1;35m│\033[0m  " << it->first << " -> " << it->second << std::endl;
			}
		}
		else
		{
			std::cout << "\033[1;33m│\033[0m \033[1;35m│\033[0m  Aucune page d'erreur configurée" << std::endl;
		}
		std::cout << "\033[1;33m│\033[0m \033[1;35m└─────────────────────┘\033[0m" << std::endl;

		// Routes
		std::cout << "\033[1;33m│\033[0m" << std::endl;
		std::cout << "\033[1;33m│\033[0m \033[1;32m┌─── ROUTES (" << server->routes.size() << ") ───┐\033[0m" << std::endl;

		for (std::vector<Route*>::const_iterator it = server->routes.begin(); it != server->routes.end(); ++it)
		{
			if (!(*it))
			{
				std::cerr << "Error: Null route pointer." << std::endl;
				continue;
			}

			Route* route = *it;

			std::cout << "\033[1;33m│\033[0m \033[1;32m│\033[0m \033[1mRoute:\033[0m " << route->path << std::endl;
			std::cout << "\033[1;33m│\033[0m \033[1;32m│\033[0m   \033[1mMéthodes:\033[0m ";

			if (route->methods.empty())
			{
				std::cout << "désactivé" << std::endl;
			}
			else
			{
				for (std::vector<std::string>::const_iterator mit = route->methods.begin(); mit != route->methods.end(); ++mit)
				{
					std::cout << *mit;
					if (mit + 1 != route->methods.end())
						std::cout << ", ";
				}
				std::cout << std::endl;
			}

			std::cout << "\033[1;33m│\033[0m \033[1;32m│\033[0m   \033[1mRacine:\033[0m " << route->root << std::endl;
			std::cout << "\033[1;33m│\033[0m \033[1;32m│\033[0m   \033[1mIndex:\033[0m " << route->index << std::endl;
			std::cout << "\033[1;33m│\033[0m \033[1;32m│\033[0m   \033[1mAutoindex:\033[0m " << (route->autoindex ? "activé" : "désactivé") << std::endl;

			if (!route->upload_dir.empty())
				std::cout << "\033[1;33m│\033[0m \033[1;32m│\033[0m   \033[1mDossier upload:\033[0m " << route->upload_dir << std::endl;

			if (!route->redirect.empty())
				std::cout << "\033[1;33m│\033[0m \033[1;32m│\033[0m   \033[1mRedirection:\033[0m " << route->redirect << std::endl;

			if (route->cgi_enabled && route->cgi)
			{
				std::cout << "\033[1;33m│\033[0m \033[1;32m│\033[0m   \033[1mCGI:\033[0m activé" << std::endl;
				std::cout << "\033[1;33m│\033[0m \033[1;32m│\033[0m     \033[1mExtensions:\033[0m" << std::endl;

				for (std::map<std::string, std::string>::const_iterator cit = route->cgi->extensions.begin(); cit != route->cgi->extensions.end(); ++cit)
				{
					std::cout << "\033[1;33m│\033[0m \033[1;32m│\033[0m       " << cit->first << " -> " << cit->second << std::endl;
				}
			}
			else
			{
				std::cout << "\033[1;33m│\033[0m \033[1;32m│\033[0m   \033[1mCGI:\033[0m désactivé" << std::endl;
			}
		}
		std::cout << "\033[1;33m│\033[0m \033[1;32m└────────────────────┘\033[0m" << std::endl;
		std::cout << "\033[1;33m└───────────────────────────────────────┘\033[0m\n" << std::endl;
	}
}

void	delete_ServerDatas(ServersDatas* data)
{
	int	i = data->server_nb;
	while (i != -1)
	{
		delete data->server[i]->error_pages;
		for (int j = data->server[i]->routes_nb; j != -1; j--)
		{
			if (data->server[i]->routes[j]->cgi)
				delete data->server[i]->routes[j]->cgi;
			delete data->server[i]->routes[j];
		}
		delete data->server[i];
		i--;
	}
	delete data;
}

ServersDatas* alloc_ServerDatas(void)
{
	ServersDatas* data = ServersDatas::getInstance();

	if (!data)
		return (NULL);
	data->server_nb = -1;
	return (data);
}

bool	cgi_check(CGI* cgi)
{
	if (cgi == NULL)
		return (0);
	if (cgi->extensions.empty())
	{
		std::cerr << "Error: CGI extensions not set." << std::endl;
		return (1);
	}
	for (std::map<std::string, std::string>::iterator it = cgi->extensions.begin(); it != cgi->extensions.end(); ++it)
	{
		if (it->first.empty() || it->second.empty())
		{
			std::cerr << "Error: CGI extension or path is empty." << std::endl;
			return (1);
		}
		if (access(it->second.c_str(), F_OK) != 0)
		{
			std::cerr << "Error: CGI path not found: " << it->second << std::endl;
			return (1);
		}
		if (access(it->second.c_str(), X_OK) != 0)
		{
			std::cerr << "Error: CGI path not executable: " << it->second << std::endl;
			return (1);
		}
	}
	return (0);
}

bool	check_paths_route(Route* route)
{
	std::string route_path;

	route_path = route->root;
	if (route_path[route_path.length() - 1] != '/')
		route->root += '/';
	else
		route_path = route->root.substr(0, route_path.length() - 1);
	if (route->path[0] != '/')
	{
		std::cerr << "Error: Route path must start with '/'" << std::endl;
		return (1);
	}
	route_path += route->path;
	if (access(route_path.c_str(), F_OK) != 0)
	{
		std::cerr << "Error: Route path not found: " << route->path << std::endl;
		return (1);
	}
	if (access(route->root.c_str(), F_OK) != 0)
	{
		std::cerr << "Error: Route root not found: " << route->root << std::endl;
		return (1);
	}
	if (cgi_check(route->cgi))
		return (1);
	return (0);
}

bool	check_error_pages(Server* server)
{
	if (server->error_pages == NULL)
		return (0);
	if (server->error_pages->pages.empty())
		return (1);
	for (std::map<std::string, std::string>::iterator it = server->error_pages->pages.begin(); it != server->error_pages->pages.end(); ++it)
	{
		if (it->first.empty() || it->second.empty())
			return (1);
		if (it->first != "400" && it->first != "401" && it->first != "402" && it->first != "403" &&
			it->first != "404" && it->first != "405" && it->first != "406" && it->first != "407" &&
			it->first != "408" && it->first != "409" && it->first != "410" && it->first != "411" &&
			it->first != "412" && it->first != "413" && it->first != "414" && it->first != "415" &&
			it->first != "416" && it->first != "417" && it->first != "418" && it->first != "421" &&
			it->first != "422" && it->first != "423" && it->first != "424" && it->first != "426" &&
			it->first != "428" && it->first != "429" && it->first != "431" && it->first != "451" &&
			it->first != "500" && it->first != "501" && it->first != "502" && it->first != "503" &&
			it->first != "504" && it->first != "505" && it->first != "506" && it->first != "507" &&
			it->first != "508" && it->first != "510" && it->first != "511")
		{
			std::cerr << "Error: Invalid error page code: " << it->first << std::endl;
			return (1);
		}

		it->second = server->routes[0]->root + it->second; //je rajoute le path du root pour chercher le dossier des pages d'erreurs
		if (access(it->second.c_str(), F_OK) != 0 || access(it->second.c_str(), R_OK) != 0)
		{
			std::cerr << "Error: Error page file not found or not readable: " << it->second << std::endl;
			return (1);
		}
	}
	return (0);
}

bool	check_value(ServersDatas* data)
{
	for (int i = 0; i <= data->server_nb; i++)
	{
		if (data->server[i]->port == -1)
		{
			std::cerr << "Error: Server " << i << " has no port set." << std::endl;
			return (false);
		}
		if (check_error_pages(data->server[i]))
		{
			std::cerr << "Error: Server " << i << " has no error pages set." << std::endl;
			return (false);
		}
		if (data->server[i]->routes_nb == -1)
		{
			std::cerr << "Error: Server " << i << " has no routes set." << std::endl;
			return (false);
		}
		for (int j = 0; j <= data->server[i]->routes_nb; j++)
		{
			if (!data->server[i]->routes[j])
			{
				std::cerr << "Error: Allocation failed." << std::endl;
			}
			if (data->server[i]->routes[j]->path.empty())
			{
				std::cerr << "Error: Server " << i << ", Route " << j << " has no path set." << std::endl;
				return (false);
			}
			if (data->server[i]->routes[j]->root.empty())
			{
				std::cerr << "Error: Server " << i << ", Route " << j << " has no root set." << std::endl;
				return (false);
			}
			if (data->server[i]->routes[j]->autoindex && !data->server[i]->routes[j]->index.empty())
			{
				std::cerr << "Error: Server " << i << ", Route " << j << " has autoindex enabled and index is configured." << std::endl;
				return (false);
			}
			if (check_paths_route(data->server[i]->routes[j]))
			{
				std::cerr << "Error: Server " << i << ", Route " << j << " has invalid paths." << std::endl;
				return (false);
			}
		}
	}
	return (true);
}

ServersDatas*	init_parsing(const std::string& file_path)
{
	ServersDatas* data = alloc_ServerDatas();

	if (!data)
		return (0);
	if (get_datas(file_path, data))
	{
		// delete_ServerDatas(data);
		return (0);
	}
	if (!check_value(data))
	{
		// delete_ServerDatas(data);
		// plus besoin -> singleton -> destructeur
		return (0);
	}
	print_server_datas(*data);
	return (data);
}
