/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldoppler <ldoppler@student.42mulhouse.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 20:26:05 by hclaude           #+#    #+#             */
/*   Updated: 2025/05/04 19:53:30 by ldoppler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

void	setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL,flags | O_NONBLOCK);
}

std::vector<struct pollfd> initPollFds(ServersDatas* ServersDatas)
{
	std::vector<struct pollfd> poll_fds;

	for (int i = 0; i <= ServersDatas->server_nb; i++)
	{
		poll_fds.push_back(ServersDatas->server[i]->serverSocket->poll_fd);
	}
	return (poll_fds);
}

bool	initServers(ServersDatas* ServersDatas)
{
	int	opt = 1;
	struct pollfd server_pollfd;
	std::map<int, ServerSocket*> port_map;

	for (size_t i = 0; i < ServersDatas->server.size(); i++)
	{
		int port = ServersDatas->server[i]->port;
		if (port_map.find(port) == port_map.end())
		{
			try
			{
					ServersDatas->server[i]->serverSocket = new ServerSocket;
			}
			catch(const std::bad_alloc& e)
			{
				std::cerr << "Error: Memory allocation failed for server socket." << std::endl;
				ServersDatas->server[i]->serverSocket = 0;
				return (false);
			}
			ServersDatas->server[i]->serverSocket->sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (ServersDatas->server[i]->serverSocket->sockfd == -1)
			{
				std::cerr << "Error : Socket creation failed" << std::endl;
				return (false);
			}
			if (setsockopt(ServersDatas->server[i]->serverSocket->sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
			{
				std::cerr << "Error : Setting socket options failed" << std::endl;
				close(ServersDatas->server[i]->serverSocket->sockfd);
				return (false);
			}
			// if (setsockopt(ServersDatas->server[i]->serverSocket->sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
			// {
			// 	std::cerr << "Error : Setting socket options failed" << std::endl;
			// 	return (false);
			// }
			ServersDatas->server[i]->serverSocket->address.sin_family = AF_INET;
			ServersDatas->server[i]->serverSocket->address.sin_addr.s_addr = INADDR_ANY;
			ServersDatas->server[i]->serverSocket->address.sin_port = htons(ServersDatas->server[i]->port);
			std::cout << "port: " << ServersDatas->server[i]->port << std::endl;
			if (bind(ServersDatas->server[i]->serverSocket->sockfd, (sockaddr*)&ServersDatas->server[i]->serverSocket->address, sizeof(ServersDatas->server[i]->serverSocket->address)) == -1)
			{
				std::cerr << "Error : Binding socket failed" << std::endl;
				close(ServersDatas->server[i]->serverSocket->sockfd);
				return (false);
			}
			if (listen(ServersDatas->server[i]->serverSocket->sockfd, SOMAXCONN) == -1)
			{
				std::cerr << "Error : Listen socket failed" << std::endl;
				close(ServersDatas->server[i]->serverSocket->sockfd);
				return (false);
			}
			setNonBlocking(ServersDatas->server[i]->serverSocket->sockfd);
			port_map[port] = ServersDatas->server[i]->serverSocket;
			memset(&server_pollfd, 0, sizeof(server_pollfd));
			server_pollfd.events = POLLIN;
			server_pollfd.fd = ServersDatas->server[i]->serverSocket->sockfd;
			server_pollfd.revents = 0;
			ServersDatas->server[i]->serverSocket->poll_fd = server_pollfd;
		}
		ServersDatas->server[i]->serverSocket = port_map[port];
	}
	std::cout << BOLD_GREEN << ServersDatas->server.size() << " Server(s) initialized successfully ✅" << END << std::endl;
	return (true);
}
