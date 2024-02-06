# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yakhay <yakhay@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/19 22:08:42 by wbouwach          #+#    #+#              #
#    Updated: 2024/02/01 13:35:38 by yakhay           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = serv

SRRC = config_file/configFile.cpp \
	server/Handling.cpp req_res_post/client_class.cpp\
	req_res_post/delete.cpp req_res_post/respons.cpp req_res_post/cgi.cpp req_res_post/post.cpp req_res_post/request.cpp req_res_post/MimeType.cpp get_method/getMethod.cpp\
	server/webserver.cpp server/main.cpp

OBJ = $(SRRC:.cpp=.o)

CPPFLAGS =   -std=c++98

all: $(NAME)

$(NAME): $(OBJ)
	c++ $(CPPFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all