#ifndef WEBSERV_HPP
# define WEBSERV_HPP

/*Colors*/
# define RED "\033[0;31m"
# define PURPLE "\033[0;35m"
# define BOLD_RED "\033[1;31m"
# define GREEN "\033[0;32m"
# define BOLD_GREEN "\033[1;32m"
# define BLUE "\033[0;34m"
# define BOLD_ORANGE "\033[1;33m"
# define ORANGE "\033[0;33m"
# define GREY "\033[1;30m"
# define BOLD_PURPLE "\033[1;35m"
# define END "\033[m"

/*Parameter for my server*/
#define PORT 8080
#define BUFFER_SIZE 254

/*Error*/
# define HEADER_200 "HTTP/1.1 200 OK\r\n" \
					"Content-Length: X\r\n" \
					"Content-Type: text/html\r\n" \
					"\r\n"
# define TEST_PAGE "HTTP/1.1 200 OK\r\n" \
					"Content-Length: 157\r\n" \
					"\r\n" \
                    "<!DOCTYPE html>\n" \
                    "<html lang=\"fr\">\n" \
                    "<head>\n" \
                    "    <meta charset=\"UTF-8\">\n" \
                    "    <title>Webserv -Home</title>\n" \
                    "</head>\n" \
                    "<body>\n" \
                    "    <h1>Hello World!</h1>\n" \
                    "</body>\n" \
                    "</html>\n"

# define ERROR_404 "HTTP/1.1 404 Not Found\r\n" \
                    "Content-Length: 302\r\n" \
                    "\r\n" \
                    "<!DOCTYPE html>\n" \
                    "<html lang=\"fr\">\n" \
                    "<head>\n" \
                    "    <meta charset=\"UTF-8\">\n" \
                    "    <title>Page non trouvée</title>\n" \
                    "</head>\n" \
                    "<body>\n" \
                    "    <h1>Erreur 404 : Page non trouvée</h1>\n" \
                    "    <p>La page que vous recherchez n'existe pas ou a été déplacée.</p>\n" \
                    "    <a href=\"/\">Retour à la page d'accueil</a>\n" \
                    "</body>\n" \
                    "</html>\n"

/*Includes*/
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <iostream>
# include <cstring>
# include <poll.h>
# include <fcntl.h>
# include <stdio.h>
# include <dirent.h>
# include <climits>

/*Class*/
# include "Client.hpp"
# include "Server.hpp"

extern bool server_is_running;

const std::string  getExtensionFile(const std::string& file);

#endif