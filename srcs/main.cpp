/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldoppler <ldoppler@student.42mulhouse.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 19:08:22 by hclaude           #+#    #+#             */
/*   Updated: 2025/05/12 08:52:29 by ldoppler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

bool server_is_running = true;

bool check_extension_file(const std::string& file)
{
	std::string extension = getExtensionFile(file);

	if (extension != ".conf")
		return (true);
	return (false);
}

void signal_handler(int signum)
{
	if (signum == SIGINT)
	{
		std::cout << std::endl << "Server shutting down..." << std::endl;
		server_is_running = false;
	}
}

int main(int ac, char **av)
{
	ServersDatas	*server;
	Client			*client;

	if (ac != 2)
	{
		std::cerr << "Error: webserv need a configurationn file." << std::endl;
		return (1);
	}
	if (check_extension_file(av[1]))
	{
		std::cerr << "Error: wrong extension file." << std::endl;
		return (1);
	}
	server = init_parsing(std::string(av[1]));
	if (server == NULL)
	{
		std::cerr << "Error: Parsing failed." << std::endl;
		return (1);
	}
	if (!initServers(server))
		return (1);

	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);

	try
	{
		client = new Client;
	}
	catch(const std::bad_alloc& e)
	{
		std::cerr << e.what() << std::endl;
	}

	launch_server(server, client);
	delete client;
	delete server;
	//delete_ServerDatas(server);
	return (0);
}
