#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <poll.h>
# include <string>
# include <sstream>
# include "webserv.hpp"
# include <unistd.h>
# include <cstdlib>
# include <sys/wait.h>
# include <stdio.h>
# include <map>
# include "Server.hpp"

// Couleurs ANSI
#define COLOR_RESET   "\033[0m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_RED     "\033[31m"
#define BOLD          "\033[1m"

enum e_ClientState { WAITING_HEADERS, WAITING_BODY, PROCESSING, CGI_IN_PROGRESS, CGI_DONE, RESPONDING };

struct t_header
{
	std::string status;
	size_t content_length;
	std::string connection;
	std::string cache_control;
	std::string	type;
};

class ClientState
{
	public:
		ClientState();
		~ClientState();
		/*Getters*/
		const e_ClientState	&getClientState() const;
		 std::string&	getBufferHeader() ;
		 std::string& getBufferBody() ;
		unsigned long long getContentLength() const;
		unsigned long long getCurrentContentLength() const;
		const std::string &getTmpFilePath() const;
		const bool &getIsFileOpened() const;
		std::ofstream &getUploadFile();
		const std::string &getResponse() const;
		const bool &getHasContentLength() const;
		unsigned long long getCurrentLengthCGI() const;
		/*Setters*/
		void	setClientState(const e_ClientState &state);
		void	setBufferHeader(const std::string &bufferHeader);
		void	setBufferBody(const std::string& bufferBody);
		void	setCurrentContentLength(unsigned long long currentContentLength);
		void	setContentLength(unsigned long long contentLength);
		void	setTmpFilePath(const std::string &filepath);
		void	setIsFileOpened(const bool isOpened);
		void	setResponse(const std::string &response);
		void	setHasContentLength(const bool hasContentLength);
		void	setCurrentLengthCGI(unsigned long long length);
		/*Others*/
		void	printInfo() const;
		bool	init_upload(int fd);
		void	append_data(const char* data, ssize_t size);
		void	finalize_upload();
		bool	is_upload_complete() const;
		bool	is_file_initialized() const;
		void	appendToBufferHeader(const char *data, ssize_t length);
		void	appendToBufferBody(const char *data, ssize_t length);
		void	reset();
	private:
		e_ClientState 		_state;
		bool				_hasContentLength;
		unsigned long long	_contentLength;
		unsigned long long	_currentContentLength;
		unsigned long long	_currentLengthCGI;
		std::string			_bufferHeader;
		std::string			_bufferBody;
		std::ofstream		_uploadFile;
		std::string			_tmpFilePath;
		bool				_fileOpened;
		std::string			_response;

};

class Client
{
	public:

		/*Canonical form*/
		Client();
		~Client();

		/*Setters*/
		void		setMethod(const std::string& method);
		void		setUrlPath(const std::string& url_path);
		void		setUrl(const std::string& url);
		void		setHost(const std::string& host);
		void		setVersion(const std::string& version);
		void		setUserAgent(const std::string& user_agent);
		void		setBody(const std::string& body);
		void		setHeader(const std::map<std::string, std::string> &header);
		void		setHeaderEntry(const std::string &key, const std::string &value);
		void		setContentLength(const std::string& content_length);
		void		setKeepAlive(bool ka);

		/*Getters*/
		const		std::string& getMethod() const;
		const		std::string& getUrlPath() const;
		const		std::string& getUrl() const;
		const		std::string& getVersion() const;
		const		std::string& getHost() const;
		const		std::string& getUserAgent() const;
		const		std::string& getBody() const;
		const		std::map<std::string, std::string> &getHeader() const;
		const		std::string& getContentLength() const;
		bool 		getKeepAlive() const;

		/*Print*/
		void		printInfo() const;

		/*Parsing*/
		int 		parseClientRequest(const std::string& recv_str, Client *client);
		void		resolveVirtualServer(Client *client, Server *server);
		int 		checkPageExists();
		std::string convertRequestForSend(Route* route, Server *server);
		std::string page_autoindex(Route* route, Server *server);
		void		setHeader(const struct t_header& param, char* &dest);
		void		initHeader(t_header& param, size_t length);

		/*CGI*/
		std::string cgi(Server *server, ClientState &state);
		std::string	findBinPath(Server *server);
		void		setHeader(const struct t_header& param,std::string& dest);
		/*Error*/
		std::string	pageErrorGenerator(std::string error, std::string msg);
		std::string pageError(int errorStatus, Server *server);
		std::string loadPersonalizeErrorPage(std::string path_to_page);
	private:
		ClientState						_state;
		std::string							_root;
		std::string							_method;
		std::string							_url_path;
		std::string							_version;
		std::string							_host;
		std::string							_user_agent;
		std::string							_accept;
		std::map<std::string, std::string>	_header;
		std::string							_body;
		std::string							_url;
		std::string							_content_length;
		bool								_ka;

};

std::string trim(const std::string &str);

#endif