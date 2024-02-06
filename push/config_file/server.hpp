/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wbouwach <wbouwach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 18:14:39 by wbouwach          #+#    #+#             */
/*   Updated: 2023/12/27 21:49:56 by wbouwach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP

# define SERVER_HPP

# include <iostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <errno.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <string.h>
# include <vector>
# include <sys/stat.h>
# include <sstream>
# include <fstream>

# define MaxClient 10
# define BUFFER_SIZE 1024
# include "../config_file/configFile.hpp"
# include "../req_res_post/request.hpp"
# include "../req_res_post/respons.hpp"
# include "../req_res_post/post.hpp"
# include "../req_res_post/macro.hpp"
# include "../get_method/getMethod.hpp"

# define BUFFER_SIZE 1024
struct sockaddr_in serv_addr;


//std::string construct_file_path(const std::string& url, manyServer* server);

#endif