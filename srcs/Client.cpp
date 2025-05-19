#include "Client.hpp"

/*Canonical form*/
Client::Client() {}

Client::~Client() {}

/*Setters*/
void	Client::setMethod(const std::string& method) { _method = method; }
void	Client::setUrlPath(const std::string& url_path) { _url_path = url_path; }
void	Client::setUrl(const std::string& url) { _url = url; }
void	Client::setHost(const std::string& host) { _host = host; }
void	Client::setVersion(const std::string& version) { _version = version; }
void	Client::setUserAgent(const std::string& user_agent) { _user_agent = user_agent; }
void    Client::setHeader(const std::map<std::string, std::string> &header) { _header = header; }
void    Client::setHeaderEntry(const std::string &key, const std::string &value) { _header[key] = value; }
void	Client::setBody(const std::string& body) { _body = body; }
void	Client::setContentLength(const std::string& content_length) { _content_length = content_length; };
void	Client::setKeepAlive(bool ka) { this->_ka = ka; }

/*Getters*/

const std::string&	Client::getMethod() const { return (_method); }
const std::string&	Client::getUrlPath() const { return _url_path; }
const std::string&	Client::getUrl() const { return _url; }
const std::string&	Client::getVersion() const { return _version; }
const std::string&	Client::getHost() const { return _host; }
const std::string&	Client::getUserAgent() const { return _user_agent; }
const std::map<std::string, std::string>	&Client::getHeader() const { return _header; }
const std::string&	Client::getBody() const { return _body; }
const std::string&	Client::getContentLength() const { return _content_length; }
bool Client::getKeepAlive() const { return this->_ka; }

void	Client::printInfo() const
{
	std::cout << BOLD_ORANGE << "📌 Client Info: " << END << std::endl;
	std::cout << ORANGE << "Method: " << _method << END << std::endl;
	std::cout << ORANGE << "URL: " << _url_path << END << std::endl;
	std::cout << ORANGE << "Host: " << _host << END << std::endl;
	std::cout << ORANGE << "Version: " << _version << END << std::endl;
	std::cout << ORANGE << "User-Agent: " << _user_agent << END << std::endl;
}


void error(const std::string &errorMessage)
{
	std::cerr << "Error : " << errorMessage << std::endl;
	throw std::runtime_error(errorMessage);
}

std::string trim(const std::string &str)
{
	size_t first = str.find_first_not_of(" \t");
	if (first == std::string::npos)
		return "";
	size_t last = str.find_last_not_of(" \t");
	return str.substr(first, last -first + 1);
}

void Client::resolveVirtualServer(Client *client, Server *server)
{

	const Server* target_server = ServersDatas::getInstance()->findServerConfig(client->getHost(), server->port);
	int index = getRouteIndex(client->getUrl(), server);

	if (!target_server)
	{
		error("No server found for host: " + _host);
		return;
	}
	_root = target_server->routes[index]->root;
	if (_url_path[0] == '/')
		_url_path = _url_path.substr(1, _url_path.length() - 1);
	if (_root[_root.length() - 1] != '/')
		_root += '/';
	_url_path = _root + _url_path;
}

int Client::parseClientRequest(const std::string& client_request, Client *client)
{
	if (!client_request.empty())
	{
		std::cout << "Request has been found!" << std::endl;
		// std::cout << RED << client_request << END << std::endl; //ici probleme
	}
	else
	{
		std::cout << RED << "EMPPPTYY" << END << std::endl;
	}
	try
	{
		if (client_request.empty())
			error("request empty");

		// if (client_request.find("../") != std::string::npos)
		// 	error("bad request");

		size_t endOfLine = client_request.find("\r\n");
		if (endOfLine == std::string::npos)
			error("empty request");

		std::string firstLine = client_request.substr(0, endOfLine);
		size_t spacePos = firstLine.find(' ');
		if (spacePos == std::string::npos)
			error("Missing Method");

		size_t questionMark = firstLine.find('?');
		size_t spacePos2 = firstLine.find(' ', spacePos + 1);
		if (spacePos2 == std::string::npos)
			error("URI or missing version");

		client->setMethod(firstLine.substr(0, spacePos));
		if (client->getMethod() != "GET" &&
			client->getMethod() != "POST" &&
			client->getMethod() != "DELETE")
			error("Unknown HTTP method");

		if (questionMark == std::string::npos)
			client->setUrl(firstLine.substr(spacePos + 1, spacePos2 - spacePos - 1));
		else
		{
			client->setUrl(firstLine.substr(spacePos + 1, questionMark - spacePos - 1));
			client->setBody(firstLine.substr(questionMark + 1, spacePos2 - questionMark - 1));
		}
		client->setVersion(firstLine.substr(spacePos2 + 1));

		if (client->getVersion() != "HTTP/1.1" && client->getVersion() != "HTTP/1.0")
			error("Invalid HTTP version");


		// bool defaultKA = (client->getVersion() == "HTTP/1.1");
    	// client->setKeepAlive(defaultKA);
		size_t pos = endOfLine + 2;
		size_t headerEnd = client_request.find("\r\n\r\n");
		if (headerEnd == std::string::npos)
			error("Request format invalid");

		std::string headerSection = client_request.substr(pos, headerEnd - pos);
		pos = 0;
		while ((endOfLine = headerSection.find("\r\n", pos)) != std::string::npos)
		{
			std::string headerLine = headerSection.substr(pos, endOfLine - pos);
			if (headerLine.empty())
				break;

			size_t sep = headerLine.find(':');
			if (sep == std::string::npos)
				error("Malformed header");

			std::string key = trim(headerLine.substr(0, sep));
			std::string value = trim(headerLine.substr(sep + 1));

			if (key.empty() || value.empty())
				error("Header Key/Value cannot be empty");

			if (key == "User-Agent")
				client->setUserAgent(value);
			else if (key == "Host")
				client->setHost(value);
			else if (key =="Content-Length")
				client->setContentLength(value);
			else if (key == "Connection") {
				std::string v = value;
				bool ka = false;
				if (client->getVersion() == "HTTP/1.1")
					ka = (v != "close");
				// else // HTTP/1.0
				// 	ka = (v == "keep-alive");
				// client->setKeepAlive(ka);
				client->setHeaderEntry(key, value);
			}
			else
				client->setHeaderEntry(key, value);
			pos = endOfLine + 2;
		}
		if (headerEnd + 4 < client_request.length() && client->getMethod() == "POST")
			client->setBody(client_request.substr(headerEnd + 4));
		client->setUrlPath(client->getUrl());
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error Parse : " << e.what();
		return -1;
	}
	return 0;
}

int Client::checkPageExists()
{
	std::cout << "testing: " << _url_path << std::endl;
	if (access(_url_path.c_str(), R_OK) < 0)
	{
		std::cout << "false" << std::endl;
		return (1);
	}
	return (0);
}

void    allocate_memory(char*& str, const char* input)
{
	str = new char[strlen(input) + 1];
	strcpy(str, input);
}

std::string concatenate(const std::string& str1, const std::string& str2) {
    return str1 + str2;
}

char *concatenate(const char* str1, const char* str2)
{
	if (!str1) str1 = "";
	if (!str2) str2 = "";
	size_t length = strlen(str1) + strlen(str2) + 1;

	char *result = new char[length];

	::strcpy(result, str1);
	::strcat(result, str2);

	return (result);
}

void    Client::initHeader(t_header& param, size_t length)
{
	std::string extension = getExtensionFile(_url_path);

	if (extension == ".html")
	{
		param.cache_control = "no-cache";
		param.connection = "keep-alive";
		param.status = "200 OK";
		param.type = "text/html";
		param.content_length = length;
	}
	else if (extension == ".css")
	{
		param.cache_control = "no-cache";
		param.connection = "keep-alive";
		param.status = "200 OK";
		param.type = "text/css";
		param.content_length = length;
	}
	else if (extension == ".php")
	{
		param.cache_control = "no-cache";
		param.connection = "keep-alive";
		param.status = "200 OK";
		param.type = "text/html";
		param.content_length = length;
	}
	else
	{
		param.cache_control = "no-cache";
		param.connection = "keep-alive";
		param.status = "200 OK";
		param.type = "text/html";
		param.content_length = length;
	}
}

char* searchAndReplace(const char* original, const char* search, const char* replace) {
	if (!original || !search || !replace) return 0; // Null safety

	size_t originalLen = strlen(original);
	size_t searchLen = strlen(search);
	size_t replaceLen = strlen(replace);

	if (searchLen == 0) return 0; // Avoid infinite loop for empty search string

	// Count occurrences of `search` in `original`
	size_t count = 0;
	const char* temp = original;
	while ((temp = strstr(temp, search))) { // Find substring
		count++;
		temp += searchLen; // Move past the found occurrence
	}

	// Calculate new string size
	size_t newLen = originalLen + count * (replaceLen - searchLen) + 1; // +1 for '\0'
	char* result = new char[newLen];
	result[0] = '\0'; // Ensure it's empty initially

	const char* currentPos = original;
	char* resultPos = result;

	// Replace occurrences
	while ((temp = strstr(currentPos, search))) {
		// Copy everything before the occurrence
		size_t lenBefore = temp - currentPos;
		strncpy(resultPos, currentPos, lenBefore);
		resultPos += lenBefore;

		// Copy replacement string
		strcpy(resultPos, replace);
		resultPos += replaceLen;

		// Move past the `search` occurrence
		currentPos = temp + searchLen;
	}

	// Copy the remaining part of the original string
	strcpy(resultPos, currentPos);

	return result; // Caller must delete[] result
}

std::string to_string(int value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

std::string	Client::pageErrorGenerator(std::string error, std::string msg)
{
	std::string ret;

	std::string body =
	"\r\n"
	"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
	"<html>\n"
	"<head>\n"
	"    <title>" + error + " - Not Found</title>\n"
	"</head>\n"
	"<body>\n"
	"    <h1>" + error + " - " + msg + "</h1>\n"
	"</body>\n"
	"</html>\n";

	std::string header = "HTTP/1.1 " + error + " " + msg + "\r\n";
	header += "Content-Type: text/html\r\n";
	header += "Content-Length: " + to_string(body.length()) + "\r\n";
	header += "\r\n";

	ret = header + body;

	return ret;
}


std::string Client::loadPersonalizeErrorPage(std::string path_to_page)
{
	int fd = open(path_to_page.c_str(), O_RDONLY);
	char	buffer[254];
	ssize_t counter;
	std::string ret;

	if (fd < 0)
	{
		std::cerr << "Error with file descriptor in the function loadPersonalizeErrorPage" << std::endl;
		return ("");
	}

	while ((counter = read(fd, buffer, sizeof(buffer) - 1)) > 0)
	{
		ret.append(buffer, counter);
	}
	close(fd);


	std::string header;
	struct t_header param;
	this->initHeader(param, ret.size());
	this->setHeader(param, header); // assume setHeader modifie `header`

	header += ret; // simple et efficace
	std::cout << "new page: " << header << std::endl;
	return (header);
}

std::string Client::pageError(int errorStatus, Server *server)
{
	std::string errorPage;

	std::cout << server->error_pages->pages[to_string(405)] << std::endl;
	if (!server->error_pages->pages[to_string(errorStatus)].empty())
	{
		std::cout << RED << "Personalized page detected!" << END << std::endl;
		return (loadPersonalizeErrorPage(server->error_pages->pages[to_string(errorStatus)]));

	}

	switch (errorStatus)
	{
		case 400:
			errorPage = pageErrorGenerator("400", "Bad Request");
			break ;
		case 401:
			errorPage = pageErrorGenerator("401", "Unauthorized");
			break ;
		case 403:
			errorPage = pageErrorGenerator("403", "Forbidden");
			break ;
		case 404:
			errorPage = pageErrorGenerator("404", "Not Found");
			break ;
		case 405:
			errorPage = pageErrorGenerator("405", "Method Not Allowed");
			break ;
		case 408:
			errorPage = pageErrorGenerator("408", "Request Timeout");
			break ;
		case 413:
			errorPage = pageErrorGenerator("413", "Payload Too Large");
			break ;
		case 500:
			errorPage = pageErrorGenerator("500", "Internal Server Error");
			break ;
		case 502:
			errorPage = pageErrorGenerator("502", "Bad Gateway");
			break ;
		case 504:
			errorPage = pageErrorGenerator("504", "Gateway Timeout");
			break ;
		case 204:
			errorPage = pageErrorGenerator("204", "No Content");
			break ;
		default:
			errorPage = pageErrorGenerator("?", "Unknown error");
	}
	return (errorPage);
}

char* sizeTToChar(size_t value) {
	static char buffer[32]; // Static buffer (not thread-safe)
	char* ptr = buffer + sizeof(buffer) - 1; // Point to the last position
	*ptr = '\0'; // Null-terminate the string

	// Handle zero case explicitly
	if (value == 0) {
		*(--ptr) = '0';
		return ptr;
	}

	// Convert number to string manually
	while (value > 0) {
		*(--ptr) = static_cast<char>('0' + (value % 10)); // Extract digit
		value /= 10;
	}

	return ptr; // Return pointer to the beginning of the string
}

std::string Client::convertRequestForSend(Route* route, Server *server)
{
	const size_t	buffer_size = 255;
	char			buffer[buffer_size];
	std::string		result;
	std::string		file_path;

	if (!isFilePath(_url))
	{
		if (!route->index.empty())
		{
			file_path = _url_path;
			if (file_path[file_path.size() - 1] != '/' && route->index[0] != '/')
				file_path += '/';
			file_path.append(route->index);
			std::cout << "file_path: " << file_path << std::endl;
		}
		else if (route->autoindex)
			return (page_autoindex(route, server));
		if (access(file_path.c_str(), R_OK) < 0)
			return (pageError(404, server));
	}
	else
		file_path = _url_path;

	int fd = open(file_path.c_str(), O_RDONLY);
	if (fd < 0)
	{
		std::cerr << "Error: file descriptor." << std::endl;
		return ("");
	}

	ssize_t counter;
	while ((counter = read(fd, buffer, buffer_size - 1)) > 0) // bah alors sale clebard on a pas protege ????
	{
		result.append(buffer, counter);
	}

	close(fd);

	std::string header;
	struct t_header param;
	this->initHeader(param, result.size());
	this->setHeader(param, header); // assume setHeader modifie `header`

	header += result; // simple et efficace

	return (header);
}

/*
void		Client::setHeader(const struct t_header& param, char* &dest)
{
	std::string result;

	std::stringstream ss;
	ss << param.content_length; //convert size_t into a std::string (for Content-Length)
	std::string tmp = ss.str();

	result = "HTTP/1.1 " + param.status + "\r\n";
	result += "Content-Length: " + tmp + "\r\n";
	result += "Content-Type: " + param.type + "\r\n";
	result += "Connection: " + param.connection + "\r\n";
	result += "Cache-Control: " + param.cache_control + "\r\n";
	result += "\r\n";

	allocate_memory(dest, result.c_str());
}*/

void Client::setHeader(const t_header& param, std::string& dest) {
    std::ostringstream ss;
    ss << param.content_length;
    
    dest = "HTTP/1.1 " + param.status + "\r\n";
    dest += "Content-Length: " + ss.str() + "\r\n";
    dest += "Content-Type: " + param.type + "\r\n";
    dest += "Connection: " + param.connection + "\r\n";
    dest += "Cache-Control: " + param.cache_control + "\r\n\r\n";
}

void	hideHeaderCGI(std::string& buf)
{
	size_t pos = buf.find("\r\n\r\n");
	//pos = buf.find("\r\n", pos + 1);

	if (pos == std::string::npos)
	{
		std::cerr << "error" << std::endl;
		return ;
	}
	buf = buf.substr(pos);
	// std::cout << GREEN << "there: " << buf << END << std::endl;
}
/*
std::string Client::findBinPath(Server *server)
{
	std::string extension = getExtensionFile(_url_path);
	char *ret = 0;

	std::cout << "bin path: " << server->routes[0]->cgi->extensions[extension] << std::endl;
	std::string bin_path = server->routes[0]->cgi->extensions[extension];
	// if (bin_path.empty())
	// 	return (0);

	// ret = new char[bin_path.size() + 1];
	// strcpy(ret, bin_path.c_str());

	return (bin_path);
}*/
unsigned long long ClientState::getCurrentLengthCGI() const
{
	return (_currentLengthCGI);
}

void	ClientState::setCurrentLengthCGI(unsigned long long size)
{
	_currentLengthCGI = size;
}

// std::string Client:: cgi(Server *server, ClientState &state, std::vector<struct pollfd>& poll_fds)
// {
// 	std::cout << BLUE << "                  =-= Starting CGI =-=" << END << std::endl;
// (void)poll_fds;
// 	state.setClientState(CGI_IN_PROGRESS);
	
//     std::string total_output;
// 	std::string header;
//     std::string path = server->routes[0]->cgi->extensions[getExtensionFile(_url_path)];
    
//     if (path.empty()) {
//         return pageError(500, server);
//     }

//     // Préparation des variables d'environnement
//     std::vector<std::string> env;
//     env.push_back("REQUEST_METHOD=" + _method);
//     env.push_back("CONTENT_LENGTH=" + _content_length);
//     env.push_back("CONTENT_TYPE=" + (_header["Content-Type"].empty() ? 
//         "application/x-www-form-urlencoded" : _header["Content-Type"]));
//     env.push_back("QUERY_STRING=" + (_method == "POST" ? "" : _body));
//     env.push_back("SCRIPT_FILENAME=" + _url_path);
//     env.push_back("REDIRECT_STATUS=200");
//     env.push_back("REQUEST_URI=" + _url);

//     std::vector<const char*> envp;
//     for (size_t i = 0; i < env.size(); ++i) {
//         envp.push_back(env[i].c_str());
//     }
//     envp.push_back(NULL);
// 	char *av[] = {(char*)path.c_str(),
// 		(char *)"-d", (char *)"max_file_uploads=1000",
// 		(char *)"-d", (char *)"max_file_uploads=1000",
// 		(char *)"-d", (char *)"post_max_size=2100G",
// 		(char *)"-d", (char *)"upload_max_filesize=20G",
// 		(char *)"-d", (char *)"memory_limit=16G",
// 		NULL};

// 	//print env variable content
// 	for (size_t i = 0; envp[i] != NULL; ++i)
// 		std::cout << PURPLE << envp[i] << END << std::endl;
// 	std::cout << std::endl;

// 	int pipefd_out[2];
// 	int pipefd_in[2];

// 	if (pipe(pipefd_out) == -1 || pipe(pipefd_in) == -1)
// 		return pageError(500, server);

// 	int id = fork();
// 	if (id == -1) {
// 		close(pipefd_out[0]); close(pipefd_out[1]);
//         close(pipefd_in[0]); close(pipefd_in[1]);
// 		return pageError(500, server);
// 	}

// 	if (id == 0)
// 	{
// 		close(pipefd_out[0]); //close read pipe
// 		close(pipefd_in[1]);

// 		if (dup2(pipefd_in[0], STDIN_FILENO) == -1)
// 			exit(EXIT_FAILURE);
// 		if (dup2(pipefd_out[1], STDOUT_FILENO) == -1)
// 			exit(EXIT_FAILURE);

// 		close(pipefd_in[0]);
// 		close(pipefd_out[1]);

// 		execve(path.c_str(),  av, (char **)&envp[0]);
// 		exit(EXIT_FAILURE);
// 	}
// 	else
// 	{
// 		close(pipefd_in[0]);
// 		close(pipefd_out[1]); //close write pipe

// 		int fd = open(state.getTmpFilePath().c_str(), O_RDONLY);
// 		if (fd < 0) {
// 			perror("open");
// 			close(pipefd_in[1]);
// 			close(pipefd_out[0]);
// 			return NULL;
// 		}
		
// 		// struct pollfd fd_poll;
// 		// fd_poll.fd = fd;
// 		// fd_poll.revents = POLLIN;
// 		// fd_poll.events = 0;


// 		// poll_fds.push_back(fd_poll);

// 		char buffer[4096];
// 		ssize_t bytes_read = 0;
// 		(void)bytes_read;
// 		// while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
// 		// 	ssize_t total_written = 0;
// 		// 	while (total_written < bytes_read) {
// 		// 		ssize_t written = write(pipefd_in[1], buffer + total_written, bytes_read - total_written);
// 		// 		if (written == -1)
// 		// 			break;
// 		// 		total_written += written;
// 		// 	}
// 		// 	if (total_written != bytes_read) {
// 		// 		// Gestion d'erreur incomplète
// 		// 		// break;
// 		// 	}
// 		// }
// 		bytes_read = read(fd, buffer, sizeof(buffer));
// 		ssize_t written = write(pipefd_in[1], buffer, bytes_read);
// 		state.setCurrentLengthCGI(state.getCurrentLengthCGI() + written);

// 		close(fd);
// 		close(pipefd_in[1]);

// 		ssize_t byte_read;
// 		char buf[4096];
		
// 		while ((byte_read = read(pipefd_out[0], buf, sizeof(buf))) > 0)
// 			total_output.append(buf, byte_read);

// 		close(pipefd_out[0]);
// 		// waitpid(id, 0, 0);
// 		wait(NULL);
// 	}

// 	hideHeaderCGI(total_output);

// 	//init my header
// 	struct t_header param;
// 	this->initHeader(param, total_output.size());
// 	this->setHeader(param, header);
// 	std::string ret = header + total_output;
// 	// header = concatenate(header, total_output.c_str());

// 	std::cout << "currentLengthCGI: " << state.getCurrentLengthCGI() << " and ContentLength: " << state.getContentLength() << std::endl;
// 	if (state.getCurrentLengthCGI() >= state.getContentLength())
// 		state.setClientState(RESPONDING);

	
// 	std::cout << BLUE << header << END << std::endl;
// 	std::cout << GREEN << "\n=-=-=-= CGI has been executed successfully! =-=-=-=" << END << std::endl;
// 	return (ret);
// }

// bool	Client::continue_cgi(Server *server, ClientState &state, std::vector<struct pollfd>& poll_fds)
// {
// 		close(pipefd_in[0]);
// 		close(pipefd_out[1]); //close write pipe

// 		int fd = open(state.getTmpFilePath().c_str(), O_RDONLY);
// 		if (fd < 0) {
// 			perror("open");
// 			close(pipefd_in[1]);
// 			close(pipefd_out[0]);
// 			return NULL;
// 		}
		
// 		// struct pollfd fd_poll;
// 		// fd_poll.fd = fd;
// 		// fd_poll.revents = POLLIN;
// 		// fd_poll.events = 0;


// 		// poll_fds.push_back(fd_poll);

// 		char buffer[4096];
// 		ssize_t bytes_read = 0;
// 		(void)bytes_read;
// 		// while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
// 		// 	ssize_t total_written = 0;
// 		// 	while (total_written < bytes_read) {
// 		// 		ssize_t written = write(pipefd_in[1], buffer + total_written, bytes_read - total_written);
// 		// 		if (written == -1)
// 		// 			break;
// 		// 		total_written += written;
// 		// 	}
// 		// 	if (total_written != bytes_read) {
// 		// 		// Gestion d'erreur incomplète
// 		// 		// break;
// 		// 	}
// 		// }
// 		bytes_read = read(fd, buffer, sizeof(buffer));
// 		ssize_t written = write(pipefd_in[1], buffer, bytes_read);
// 		state.setCurrentLengthCGI(state.getCurrentLengthCGI() + written);

// 		close(fd);
// 		close(pipefd_in[1]);

// 		ssize_t byte_read;
// 		char buf[4096];
		
// 		while ((byte_read = read(pipefd_out[0], buf, sizeof(buf))) > 0)
// 			total_output.append(buf, byte_read);

// 		close(pipefd_out[0]);
// 		// waitpid(id, 0, 0);
// 		wait(NULL);
// 	}

// 	hideHeaderCGI(total_output);

// 	//init my header
// 	struct t_header param;
// 	this->initHeader(param, total_output.size());
// 	this->setHeader(param, header);
// 	std::string ret = header + total_output;
// 	// header = concatenate(header, total_output.c_str());

// 	std::cout << "currentLengthCGI: " << state.getCurrentLengthCGI() << " and ContentLength: " << state.getContentLength() << std::endl;
// 	if (state.getCurrentLengthCGI() >= state.getContentLength())
// 		state.setClientState(RESPONDING);

	
// 	std::cout << BLUE << header << END << std::endl;
// 	std::cout << GREEN << "\n=-=-=-= CGI has been executed successfully! =-=-=-=" << END << std::endl;
// 	return (true);
// }

bool Client:: start_cgi(Server *server, ClientState &state, std::vector<struct pollfd>& poll_fds)
{
	std::cout << BLUE << "                  =-= Starting CGI =-=" << END << std::endl;

	(void)poll_fds;
	state.setClientState(CGI_IN_PROGRESS);
	
    std::string total_output;
	std::string header;
    std::string path = server->routes[0]->cgi->extensions[getExtensionFile(_url_path)];
    
    if (path.empty()) {
        return false;
    }

    // Préparation des variables d'environnement
    std::vector<std::string> env;
    env.push_back("REQUEST_METHOD=" + _method);
    env.push_back("CONTENT_LENGTH=" + _content_length);
    env.push_back("CONTENT_TYPE=" + (_header["Content-Type"].empty() ? 
        "application/x-www-form-urlencoded" : _header["Content-Type"]));
    env.push_back("QUERY_STRING=" + (_method == "POST" ? "" : _body));
    env.push_back("SCRIPT_FILENAME=" + _url_path);
    env.push_back("REDIRECT_STATUS=200");
    env.push_back("REQUEST_URI=" + _url);

    std::vector<const char*> envp;
    for (size_t i = 0; i < env.size(); ++i) {
        envp.push_back(env[i].c_str());
    }
    envp.push_back(NULL);
	char *av[] = {(char*)path.c_str(),
		(char *)"-d", (char *)"max_file_uploads=1000",
		(char *)"-d", (char *)"max_file_uploads=1000",
		(char *)"-d", (char *)"post_max_size=2100G",
		(char *)"-d", (char *)"upload_max_filesize=20G",
		(char *)"-d", (char *)"memory_limit=16G",
		NULL};

	//print env variable content
	for (size_t i = 0; envp[i] != NULL; ++i)
		std::cout << PURPLE << envp[i] << END << std::endl;
	std::cout << std::endl;

	int pipefd_out[2];
	int pipefd_in[2];

	if (pipe(pipefd_out) == -1 || pipe(pipefd_in) == -1)
		return false;

	int id = fork();
	if (id == -1) {
		close(pipefd_out[0]); close(pipefd_out[1]);
        close(pipefd_in[0]); close(pipefd_in[1]);
		return false;
	}

	if (id == 0)
	{
		close(pipefd_out[0]); //close read pipe
		close(pipefd_in[1]);

		if (dup2(pipefd_in[0], STDIN_FILENO) == -1)
			exit(EXIT_FAILURE);
		if (dup2(pipefd_out[1], STDOUT_FILENO) == -1)
			exit(EXIT_FAILURE);

		close(pipefd_in[0]);
		close(pipefd_out[1]);

		execve(path.c_str(),  av, (char **)&envp[0]);
		exit(EXIT_FAILURE);
	}
	return (true);
}
