/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_data.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hclaude <hclaude@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 00:52:04 by hclaude           #+#    #+#             */
/*   Updated: 2025/04/22 01:15:18 by hclaude          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

std::string to_lower(const std::string& str) {
	std::string res = str;
	for (size_t i = 0; i < res.size(); ++i) {
		res[i] = std::tolower(res[i]);
	}
	return res;
}

const Server* ServersDatas::findServerConfig(const std::string& host, int port) const {
	const Server* first_on_port = NULL;
	std::string clean_host = host.substr(0, host.find(':'));

	for (std::vector<Server*>::const_iterator it = server.begin(); it != server.end(); ++it)
	{
		if ((*it)->port != port) continue;

		if (!first_on_port) first_on_port = *it;

		for (size_t i = 0; i < (*it)->server_names.size(); ++i) {
			std::string server_name = (*it)->server_names[i];
			std::transform(server_name.begin(), server_name.end(), server_name.begin(), ::tolower);
			if (server_name == clean_host) {
				return *it;
			}
		}
	}

	return first_on_port;
}

void ServersDatas::printConfig() const {
	std::cout << "=== Liste des Serveurs ===" << std::endl;
	for (size_t i = 0; i < server.size(); ++i) {
		std::cout << "Serveur " << i + 1 << ":" << std::endl;
		std::cout << "  Port: " << server[i]->port << std::endl;
		std::cout << "  Noms: ";
		for (size_t j = 0; j < server[i]->server_names.size(); ++j) {
			std::cout << server[i]->server_names[j] << " ";
		}
		std::cout << std::endl << std::endl;
	}
}
