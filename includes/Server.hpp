#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>
# include <unistd.h>
# include <sys/errno.h>
# include <stdlib.h>
# include <stdio.h>
# include <fcntl.h>
# include <cstring>
# include <string>
# include <fstream>
# include <sstream>
# include <algorithm>
# include <netinet/in.h>
# include <poll.h>

# define DEFAULT_PATH "conf/default.conf"

class Client;

// Enum representing the types of tokens used during parsing
enum e_token
{
	BRACKET_OPEN,  // [
	BRACKET_CLOSE, // ]
	BRACE_OPEN,    // {
	BRACE_CLOSE,   // }
	KEY,           // Key in a key-value pair
	COLON,         // :
	VALUE,         // Value in a key-value pair
	COMMA,         // ,
	UNKNOWN        // Unknown token type
};

// Structure representing a token with its type and value
struct Token
{
	e_token		type;  // Type of the token
	std::string	value; // Value of the token
};

// Structure for managing CGI extensions and their associated programs
struct CGI
{
	std::map<std::string, std::string>	extensions; // Example: ".php" -> "/usr/bin/php-cgi"
	~CGI();
};

// Structure representing a route in the server
struct Route
{
	std::string					path;        // Path of the route
	std::vector<std::string>	methods;     // Allowed HTTP methods (GET, POST, etc.)
	std::string					root;        // Root directory for this route
	std::string					index;       // Default index file
	bool						autoindex;   // Enable or disable autoindex
	std::string					upload_dir;  // Directory for file uploads
	std::string					redirect;    // URL for redirection
	bool						cgi_enabled; // Indicates if CGI is enabled for this route
	CGI*						cgi;         // Pointer to the CGI configuration
	~Route();
};

// Structure for managing custom error pages
struct ErrorPages
{
	std::map<std::string, std::string> pages; // Example: "404" -> "404.html"
	~ErrorPages();
};

struct ServerSocket
{
	pollfd						poll_fd;           // List of poll file descriptors
	int							poll_nb;           // Number of poll file descriptors
	int							sockfd;            // Socket file descriptor
	sockaddr_in					address;           // Socket address structure
	int							addrlen;           // Length of the address structure
	~ServerSocket();
};

// Structure representing a server
struct Server
{
	std::string					host;                 // IP address or hostname
	int							port;                 // Listening port
	std::vector<std::string>	server_names;         // Associated domain names
	bool						server_default;       // Indicates if this is the default server
	ErrorPages*					error_pages;          // Custom error pages
	std::vector<Route*>			routes;               // List of configured routes
	int							routes_nb;            // Number of routes
	int							client_max_body_size; // Maximum size of the request body
	ServerSocket*				serverSocket;
	~Server();
};

// Structure containing data for all configured servers
struct ServersDatas
{
	~ServersDatas();
	int						server_nb; // Total number of servers
	std::vector<Server*>	server;    // List of servers

	const Server*		findServerConfig(const std::string& host, int port) const;
	static ServersDatas	*getInstance();
	void				printConfig() const;

	private:
		ServersDatas();
		static ServersDatas *_instance;
		ServersDatas(const ServersDatas&);
		ServersDatas& operator=(const ServersDatas&);
};

// Parsing
ServersDatas*		init_parsing(const std::string& file_path); // Initialize parsing from a configuration file
int					assign_routes(size_t& i, std::vector<Token> words, Server* server); // Assign routes to a server
int					parse_this_shit(std::vector<Token> words, ServersDatas* data); // Parse tokens to populate server data

// CGI
int					assign_cgi(size_t& i, std::vector<Token> words, Server* server); // Configure CGI for a server

// Tokenizer
std::vector<Token>	tokenize(std::string& line); // Tokenize a line into tokens

// Utils
int 				alloc_server(ServersDatas* data); // Allocate a new server in ServersDatas
int					alloc_error_pages(Server* server); // Allocate error pages for a server
int					alloc_route(Server* server); // Allocate a new route for a server
std::string			clear_line(const std::string&	buffer); // Clean a line (remove unnecessary spaces, etc.)
bool				syntax_check(std::string& line); // Check the syntax of a line
bool				error_check(std::vector<Token> words); // Check for errors in tokens
int					ft_atoi(const std::string& nptr); // Convert a string to an integer
void				delete_ServerDatas(ServersDatas* data); // Free memory allocated for ServersDatas
bool				is_server(int fd, ServersDatas* serversdatas);
std::string			extract_path(const std::string& url);
int					getRouteIndex(const std::string& url, Server* server);
bool				isMethodhere(std::string& method, Route* route);
bool				isFilePath(const std::string& path);

// Initialization of servers
bool						initServers(ServersDatas* ServersDatas);
void						setNonBlocking(int fd);
std::vector<struct pollfd>	initPollFds(ServersDatas* ServersDatas);

// launch

int	launch_server(ServersDatas* serversdatas, Client* client);

#endif