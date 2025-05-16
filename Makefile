SRC = ./srcs/main.cpp \
./srcs/Client.cpp \
./srcs/utils/utils_parsing.cpp \
./srcs/parsing_conf/tokenizer.cpp ./srcs/parsing_conf/routes_parsing.cpp ./srcs/parsing_conf/parsing.cpp \
./srcs/parsing_conf/init_parsing.cpp ./srcs/parsing_conf/utils/utils_parsing.cpp ./srcs/parsing_conf/cgi_parsing.cpp \
./srcs/server/init_server.cpp  ./srcs/server/utils.cpp ./srcs/server/main_server.cpp \
./srcs/server/server_data.cpp ./srcs/server/delete.cpp ./srcs/utils_client.cpp\
./srcs/ClientState.cpp\

INCLUDES = -I ./includes/
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3 $(INCLUDES)
OBJ = $(SRC:.cpp=.o)
NAME = webserv

all : $(NAME)

$(NAME) : $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
clean :
	rm -rf $(OBJ)
fclean : clean
	rm -rf $(NAME)
re : fclean all $(NAME)

.PHONY: all fclean clean re
