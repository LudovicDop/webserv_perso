/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hclaude <hclaude@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 21:00:38 by hclaude           #+#    #+#             */
/*   Updated: 2025/04/28 21:02:03 by hclaude          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

#include "Server.hpp"

CGI::~CGI()
{
	extensions.clear();
}

Route::~Route()
{
	delete cgi;
}

ErrorPages::~ErrorPages()
{
	pages.clear();
}

ServerSocket::~ServerSocket()
{
	close(sockfd);
}

Server::~Server()
{
	delete error_pages;
	for (size_t i = 0; i < routes.size(); i++)
	{
		delete routes[i];
	}
	delete serverSocket;
}

ServersDatas::~ServersDatas()
{
	for (size_t i = 0; i < server.size(); i++)
	{
		delete server[i];
	}
	server.clear();
}
