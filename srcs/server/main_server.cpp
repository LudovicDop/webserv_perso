/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldoppler <ldoppler@student.42mulhouse.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 14:32:13 by hclaude           #+#    #+#             */
/*   Updated: 2025/05/17 00:06:38 by ldoppler         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "webserv.hpp"
#include "Client.hpp"

int	isCgiRequired(Server *server, Client *client)
{
	int	i;
	std::string	url;

	url = client->getUrl();
	std::cout << url << std::endl;
	i = getRouteIndex(url, server);
	
	std::cout << "CGI enabled ? " << (server->routes[i]->cgi_enabled ? "yes" : "no") << std::endl;
	std::string ext = getExtensionFile(client->getUrlPath());
	std::cout << "Requested file extension: " << ext << std::endl;
	
	if (i == -1)
		return (1);

	else if (!(server->routes[i]->cgi_enabled) || server->routes[i]->cgi->extensions.find(getExtensionFile(client->getUrlPath())) == server->routes[i]->cgi->extensions.end())
	{
		std::cout << "port: " << server->port << std::endl;
		std::cout << "no cgi" << std::endl;
		return (1);
	}
	std::cout << "cgi needed: " << server->routes[i]->cgi_enabled << std::endl;
	return (0);
}

bool	new_client(int i, std::vector<struct pollfd>& poll_fds, std::map<int, int>& client_data, std::map<int, ClientState*>& client_state)
{
	int			client_fd;
	sockaddr_in	client_address;
	socklen_t	client_address_len;
	pollfd		client_tmp;

	client_address_len = sizeof(client_address);
	client_fd = accept(poll_fds[i].fd, (sockaddr*)&client_address, &client_address_len);
	if (client_fd < 0)
		return (false);

	setNonBlocking(client_fd);
	client_data[client_fd] = i;

	ClientState*	newClient = new ClientState();
	client_state[client_fd] = newClient;
	client_state[client_fd]->setCurrentLengthCGI(0);
	
	client_tmp.fd = client_fd;
	client_tmp.events = POLLIN;
	client_tmp.revents = 0;
	poll_fds.push_back(client_tmp);
	return (true);
}

bool get_request_body(int fd, const std::string& last, std::map<int, ClientState*>& client_state) {
	ClientState* state = client_state[fd];

	if (!state->is_file_initialized()) {
		if (!state->init_upload(fd)) {
			std::cerr << "Failed to open upload file" << std::endl;
			return false;
		}
	}
	if (!last.empty())
		state->append_data(last.c_str(), last.size());
	
	if (state->getContentLength() == 0) {
		state->finalize_upload();
		return true;
	}

	char buffer[1024 * 1024];
	ssize_t bytes_received = recv(fd, buffer, sizeof(buffer), 0);

	if (bytes_received > 0)
		state->append_data(buffer, bytes_received);
	else if (bytes_received == 0) {
		state->finalize_upload();
		return true;
	}

	if (state->is_upload_complete())
		state->finalize_upload();

	return true;
}

bool is_valid_number(const std::string& str) {
	if (str.empty()) return false;
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
		if (!isdigit(*it)) return false;
	return true;
}

bool getContentLength(const std::string& header, unsigned long long &content_length) {
	std::string key = "Content-Length:";
	size_t pos = header.find(key);
	if (pos == std::string::npos)
		return false;
	size_t end = header.find("\r\n", pos);
	std::string len_str = trim(header.substr(pos + key.length(), end - (pos + key.length())));
	if (len_str.empty())
		return false;
	if (!is_valid_number(len_str))
		return false;
	unsigned long long result = 0;
	for (size_t i = 0; i < len_str.size(); ++i) {
		if (result > ULLONG_MAX / 10) {
			return false;
		}
		result = result * 10 + (len_str[i] - '0');
	}
	content_length = result;
	return true;
}


int	get_header(int fd, std::map<int, ClientState*> &client_state)
{
	ClientState* state = client_state[fd];
	if (!state) {
		std::cerr << RED << "Invalid client state" << END << std::endl;
		return -1;
	}
	
	char	buffer[8 * 1024];
	ssize_t	bytes_received = 0;
	
	if (state->getClientState() == WAITING_HEADERS)
	{
		// state->printInfo();
		bytes_received = recv(fd, buffer, sizeof(buffer), 0);
		if (bytes_received == 0)
		{
			std::cout << std::endl;
			std::cout << GREY << "empty request" << END << std::endl;
			return (0);
		}
		else if (bytes_received < 0)
			return 1;

		state->appendToBufferHeader(buffer, bytes_received);
		const std::string &raw = state->getBufferHeader();
		size_t header_end = state->getBufferHeader().find("\r\n\r\n");
		if (header_end == std::string::npos)
			return 1;

		std::string header = raw.substr(0, header_end + 4);
		std::string body = raw.substr(header_end + 4);
		state->setBufferHeader(header);
		
		unsigned long long content_length = 0;
		state->setHasContentLength(getContentLength(header, content_length));
		state->setContentLength(state->getHasContentLength() ? content_length : 0);
		
		state->setClientState(WAITING_BODY);
		get_request_body(fd, body, client_state);
		memset(buffer, 0, sizeof(buffer));
		
		return 1;
	}

	if (state->getClientState() == WAITING_BODY)
		get_request_body(fd, "", client_state);

    if (state->getClientState() != WAITING_HEADERS)
    {
        std::cout << "\033[1m[Content-Length Check] \033[0m"
                  << "Received: \033[36m"
                  << state->getCurrentContentLength()
                  << "\033[0m"
                  << " / Expected: \033[32m"
                  << state->getContentLength()
                  << "\033[0m"
                  << " -> Status: ";
        state->printInfo();
    }
	return (1);
}


std::string redirect(std::string& link)
{
	std::string response;
	std::string body = "<html><head><title>301 Moved Permanently</title></head><body><h1>Moved Permanently</h1><p>Ressource moved to <a href=\"" + link + "\">this address</a>.</p></body></html>";

	std::stringstream ss;
	ss << body.length();

	response.append("HTTP/1.1 301 Moved Permanently\r\n");
	response.append("Location: " + link + "\r\n");
	response.append("Content-Type: text/html\r\n");
	response.append("Content-Length: " + ss.str() + "\r\n");
	response.append("\r\n");
	response.append(body);

	return (response);
}

std::string	execGET(Client* client, int index_server, ServersDatas* serverdatas, int fd, std::map<int, ClientState*> client_state)
{
	std::string	response;
	int			i;

	i = getRouteIndex(client->getUrl(), serverdatas->server[index_server]);
	if (i == -1)
	{
		client_state[fd]->setClientState(CGI_DONE);
		return (client->pageError(405, serverdatas->server[index_server]));
	}

	if (!serverdatas->server[index_server]->routes[i]->redirect.empty())
	{
		client_state[fd]->setClientState(CGI_DONE);
		response = redirect(serverdatas->server[index_server]->routes[i]->redirect);
		return (response);
	}

	if (!isMethodhere((std::string&)client->getMethod(), serverdatas->server[index_server]->routes[i]))
	{
		client_state[fd]->setClientState(CGI_DONE);
		return (client->pageError(405, serverdatas->server[index_server]));
	}

	if (!client->checkPageExists())
	{
		if (!isCgiRequired(serverdatas->server[index_server], client))
			response = client->cgi(serverdatas->server[index_server], *client_state[fd]);
		else
		{
			response = client->convertRequestForSend(serverdatas->server[index_server]->routes[i], serverdatas->server[index_server]);
			client_state[fd]->setClientState(CGI_DONE);
		}
	}
	else
	{
		response = client->pageError(404, serverdatas->server[index_server]);
		client_state[fd]->setClientState(CGI_DONE);
	}

	if (client_state[fd]->getClientState() == CGI_DONE)
		client_state[fd]->setClientState(RESPONDING);
	client_state[fd]->printInfo();
	return (response);
}

std::string	execPOST(Client* client, int index_server, ServersDatas* serverdatas, int fd, std::map<int, ClientState*> client_state)
{
	std::string	response;
	int			i;

	i = getRouteIndex(client->getUrl(), serverdatas->server[index_server]);
	if (i == -1)
	{
		client_state[fd]->setClientState(CGI_DONE);
		return (client->pageError(405, serverdatas->server[index_server]));
	}

	if (!serverdatas->server[index_server]->routes[i]->redirect.empty())
	{
		client_state[fd]->setClientState(CGI_DONE);
		response = redirect(serverdatas->server[index_server]->routes[i]->redirect);
		return (response);
	}

	if (!isMethodhere((std::string&)client->getMethod(), serverdatas->server[index_server]->routes[i]))
	{
		client_state[fd]->setClientState(CGI_DONE);
		return (client->pageError(405, serverdatas->server[index_server]));
	}

	if (!client->checkPageExists())
	{
		if (!isCgiRequired(serverdatas->server[index_server], client))
		{
			response = client->cgi(serverdatas->server[index_server], *client_state[fd]);
		}
		else
		{
			response = client->convertRequestForSend(serverdatas->server[index_server]->routes[i], serverdatas->server[index_server]);
			client_state[fd]->setClientState(CGI_DONE);
		}
	}
	else
	{
		response = client->pageError(404, serverdatas->server[index_server]);
		client_state[fd]->setClientState(CGI_DONE);
	}
	// client_state[fd]->setClientState(RESPONDING);
	// client_state[fd]->printInfo();
	return (response);
}

std::string urlDecode(const std::string& str) {
	std::string result;
	char ch;
	std::string::size_type i, ii;
	for (i = 0; i < str.length(); i++) {
		if (str[i] == '%') {
			if (i + 2 < str.length()) {
				std::istringstream iss(str.substr(i + 1, 2));
				iss >> std::hex >> ii;
				ch = static_cast<char>(ii);
				result += ch;
				i += 2;
			}
		} else if (str[i] == '+') {
			result += ' ';
		} else {
			result += str[i];
		}
	}
	return result;
}

std::string	execDELETE(Client* client, int index_server, ServersDatas* serverdatas)
{
	std::string	response;
	int			i;
	std::string filepath = client->getUrlPath();
	filepath = urlDecode(filepath);

	i = -1;
	std::cout << RED << "Delete in progress..." << END << std::endl;

	i = getRouteIndex(client->getUrl(), serverdatas->server[index_server]);
	if (i == -1)
		return (client->pageError(405, serverdatas->server[index_server]));

	if (!serverdatas->server[index_server]->routes[i]->redirect.empty())
	{
		response = redirect(serverdatas->server[index_server]->routes[i]->redirect);
		return (response);
	}
	std::cout << filepath << std::endl;
	if (!isMethodhere((std::string&)client->getMethod(), serverdatas->server[index_server]->routes[i]))
		return (client->pageError(405, serverdatas->server[index_server]));

	if (access(filepath.c_str(), F_OK) != 0)
		return client->pageError(404, serverdatas->server[index_server]);

	if (access(filepath.c_str(), W_OK) != 0)
		return (client->pageError(403, serverdatas->server[index_server]));

	if (remove(filepath.c_str()) != 0)
		return client->pageError(500, serverdatas->server[index_server]);
	return (client->pageError(204, serverdatas->server[index_server]));
}

bool	process_request(Client* client, ServersDatas* serversdatas, size_t& i, std::map<int, int>& client_data, std::vector<struct pollfd>& poll_fds, std::map<int, ClientState*> client_state)
{
	std::string	request;
	int			index_server;
	int			bytes_received = 0;
	std::string	response;

	if (!client || !serversdatas || i >= poll_fds.size()) {
		std::cerr << "Invalid parameters" << std::endl;
		return false;
	}

	int fd = poll_fds[i].fd;
	if (client_state.find(fd) == client_state.end()) {
		std::cerr << "No client state for FD " << fd << std::endl;
		return false;
	}
	ClientState *state = client_state[fd];

	if (state->getClientState() != CGI_IN_PROGRESS)
	{
		bytes_received = get_header(fd, client_state);
	}
	// std::cout << "(process_request) bytes_received: " << bytes_received << std::endl;
	// sleep(5);
	// bytes_received = get_header(fd, client_state);
	if (bytes_received <= 0 || (poll_fds[i].revents & (POLLERR | POLLHUP)))
	{
		close(fd);
		client_data.erase(fd);
		poll_fds.erase(poll_fds.begin() + i);
		// if (i > 0)
		// 	i--;
		return (true);
	}

	// std::cout << BLUE << "REQUEST :" << std::endl << request << END << std::endl;

	index_server = client_data[fd];
	if (index_server < 0 || index_server >= (int)serversdatas->server.size())
	{
		std::cerr << "Error: invalid server index" << std::endl;
		std::string error_response = client->pageError(500, serversdatas->server[index_server]);
		send(fd, error_response.c_str(), error_response.size(), 0);
		return (false);
	}
	if (state->getClientState() == PROCESSING || state->getClientState() == CGI_IN_PROGRESS)
	{
		if (!state->getBufferHeader().empty())
		{
			std::cout << "(process_request) Processing, parseClientHeader" << std::endl;
			if (state->getClientState() != CGI_IN_PROGRESS )
			{
				if (client->parseClientRequest(state->getBufferHeader(), client) == -1)
				{
					std::cout << std::endl;
					std::cout << "=-=-=-=-=-=-=-=-=-=-=" << std::endl;
					std::cout << BOLD_RED << "Parsing just failed." << END << std::endl;
					std::cout << state->getBufferHeader() << std::endl;
					std::cout << "=-=-=-=-=-=-=-=-=-=-=" << std::endl;
					return (-1);
	
				}	
			}
			state->setBufferHeader("");
			client->resolveVirtualServer(client, serversdatas->server[index_server]);
			const std::string method = client->getMethod();

			if (method == "GET")
				response = execGET(client, index_server, serversdatas, fd, client_state);
			else if (method == "POST")
				response = execPOST(client, index_server, serversdatas, fd, client_state);
			else if (method == "DELETE")
			{
				response = execDELETE(client, index_server, serversdatas);
				state->setClientState(RESPONDING);
			}
			else
				response = client->pageError(405, serversdatas->server[index_server]);
			// client_state[fd]->setClientState(RESPONDING);
		}
		// std::cout << "\033[0;33mMethod-> " << client->getMethod() << ", File-> " << client->getUrlPath() << "\033[m" << std::endl;

		// sleep(5);
		


	}

	if (state->getClientState() == RESPONDING)
	{
		// std::string connLine = "Connection: "
		// 	+ std::string(client->getKeepAlive() ? "keep-alive" : "close")
        //     + "\r\n";
        // size_t hdrEnd = response.find("\r\n\r\n");
        // if (hdrEnd != std::string::npos) {
        //     response.insert(hdrEnd + 2, connLine);
        // } else {
        //     response = connLine + "\r\n" + response;
        // }
		if (send(fd, response.c_str(), response.size(), 0) <= 0)
		{
			std::cerr << "Error sending response: " << strerror(errno) << std::endl;
			close(fd);
			client_data.erase(fd);
			poll_fds.erase(poll_fds.begin() + i);
		}
        // if (client->getKeepAlive())
        // {
        //     state->reset();
        //     state->setClientState(WAITING_HEADERS);
        // }
        // else
        // {
            close(fd);
            // delete state;
            client_state.erase(fd);
            client_data.erase(fd);
            poll_fds.erase(poll_fds.begin() + i);
        // }
		std::cout << BOLD_GREEN << "RESET " << fd << END << std::endl;
	}
	std::cout << "ENDDDD" << std::endl;
	return (true);
}

void	close_poll_fds(std::vector<struct pollfd>& poll_fds)
{
	for (size_t i = 0; i < poll_fds.size(); i++)
	{
		if (poll_fds[i].fd != -1)
			close(poll_fds[i].fd);
	}
}

int	launch_server(ServersDatas* serversdatas, Client* client)
{
	std::vector<struct pollfd>	poll_fds;
	std::map<int, int>			client_data;

	std::map<int, ClientState*>		client_state; //file descriptor client, info plus complète

	int							ret = 0;

	poll_fds = initPollFds(serversdatas);
	if (poll_fds.empty())
	{
		std::cerr << "Error: No server socket initialized";
		return (1);
	}

	while (server_is_running)
	{
		ret = poll(poll_fds.data(), poll_fds.size(), 500);
		if (ret == -1)
		{
			std::cerr << "Error: poll failed." << std::endl;
			close_poll_fds(poll_fds);
			return (1);
		}
		for (size_t i = poll_fds.size(); i-- > 0 ;)
		{
			if (!(poll_fds[i].revents & POLLIN))
				continue;
			if (is_server(poll_fds[i].fd, serversdatas))
			{
				std::cout << "New connection detected on server fd: " << poll_fds[i].fd << std::endl;
				if (!new_client(i, poll_fds, client_data, client_state))
				{
					std::cerr << RED << "Warning: Failed to accept new client on fd "
						<< poll_fds[i].fd << END << std::endl;
					continue;
				}
			}
			else
			{
				if (!process_request(client, serversdatas, i, client_data, poll_fds, client_state))
				{
					std::cerr << RED << "Warning: failed to process client request on fd "
						 << poll_fds[i].fd << END << std::endl;
					continue;
				}
			}
		}
	}
	close_poll_fds(poll_fds);
	return (0);
}

