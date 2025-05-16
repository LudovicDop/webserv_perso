/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hclaude <hclaude@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 14:19:37 by hclaude           #+#    #+#             */
/*   Updated: 2025/04/25 18:39:04 by hclaude          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

bool	is_server(int fd, ServersDatas* serversdatas)
{
	for (int i = 0; i <= serversdatas->server_nb; i++)
	{
		if (fd == serversdatas->server[i]->serverSocket->sockfd)
			return (true);
	}
	return (false);
}

//std::string	extract_path(const std::string& url)
//{
//	std::string					path;
//	std::string					word;
//	int							count;
//	std::string::const_iterator	it;

//	count = 0;
//	it = url.begin();
//	while (it != url.end() && count != 3)
//	{
//		if (*it == '/')
//			count++;
//		it++;
//	}
//	path.append(1, '/');
//	for (; it != url.end(); it++)
//	{
//		while (it != url.end() && (isalnum(*it) || *it == '-' || *it == '_' || *it == '~'))
//		{
//			word.append(1, *it);
//			it++;
//		}
//		if (*it == '/')
//		{
//			path.append(word);
//			word.clear();
//			word.append(1, '/');
//		}
//		else if (*it == '.')
//			return (path);
//		else
//		{
//			path.append(word);
//			return (path);
//		}
//	}
//	return (path);
//}

bool isFilePath(const std::string& path)
{
	if (!path.empty() && path[path.length() - 1] == '/')
		return false;

	size_t lastSlash = path.find_last_of('/');
	std::string lastSegment = (lastSlash != std::string::npos) ? path.substr(lastSlash + 1) : path;

	return lastSegment.find('.') != std::string::npos;
}

bool	isMethodhere(std::string& method, Route* route)
{
	for (size_t i = 0; i < route->methods.size(); i++)
	{
		if (method == route->methods[i])
			return (true);
	}
	return (false);
}

int	getRouteIndex(const std::string& url, Server* server)
{
	std::string	best_match;
	int			index;

	if (server->routes_nb == -1)
		return (-1);
	index = -1;
	for (int i = 0; i <= server->routes_nb; i++)
	{
		if (url.find(server->routes[i]->path) == 0 && server->routes[i]->path.size() > best_match.size())
		{
			best_match = server->routes[i]->path;
			index = i;
		}
	}
	if (index != -1)
	{
		//std::cout << "Route correspondante (depuis conf) : " << server->routes[index]->path << std::endl;
	}
	//std::cout << "Route trouvee : " << best_match << std::endl;
	return (index);
}
