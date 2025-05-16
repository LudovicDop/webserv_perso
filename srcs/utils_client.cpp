/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_client.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldoppler <ldoppler@student.42mulhouse.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 19:20:04 by hclaude           #+#    #+#             */
/*   Updated: 2025/04/28 10:58:48 by ldoppler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

std::string Client::page_autoindex(Route* route, Server *server)
{
	std::string html;
	std::string dirPath = _url_path;
	DIR* dir;
	dirent* entry;
	std::string response;

	dir = opendir(dirPath.c_str());
	if (!dir) {
		return (pageError(404, server));
	}

	html = "<html>\n<head>\n<title>Index of " + _url + "</title>\n";
	html += "</head>\n<body>\n<h1>Index of " + _url + "</h1>\n<ul>\n";

	if (_url != "/")
		html += "        <li><a href=\"../\">../</a></li>\n";

	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		std::string	path_file;

		if (name[0] == '.')
			continue;

		if (route->path[route->path.size() - 1] == '/')
			path_file = route->path + name;
		else
			path_file = route->path + '/' + name;

		if (entry->d_type == DT_DIR)
			html += "        <li><a href=\"" + path_file + "/\">" + name + "/</a></li>\n";
		else
			html += "        <li><a href=\"" + path_file + "\">" + name + "</a></li>\n";
	}

	html += "    </ul>\n"
			"</body>\n"
			"</html>";

	closedir(dir);

	std::stringstream ss;
	ss << html.length();

	response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + ss.str() + "\r\n";
	response += "\r\n";
	response += html;

	return (response);
}
