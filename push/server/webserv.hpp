/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yakhay <yakhay@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 18:22:50 by wbouwach          #+#    #+#             */
/*   Updated: 2024/02/06 23:45:12 by yakhay           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

# define BUFFER_SIZE 1024
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
# include <netdb.h>
# include <list>
# include <fstream>
# include <signal.h>
# include <dirent.h>
# include <sys/time.h>
#include <fcntl.h>

# include "../config_file/configFile.hpp"
# include "../req_res_post/request.hpp"
# include "../req_res_post/post.hpp"
# include "../req_res_post/macro.hpp"
# include "../req_res_post/cgi.hpp"
# include "../req_res_post/MimeType.hpp"
# include "../req_res_post/respons.hpp"
# include "../req_res_post/client_class.hpp"
# include "../req_res_post/delete.hpp"
# include "../get_method/getMethod.hpp"

/*

class Client
{
    private :
        int clientSocket;
        int idServerConnectedTo;
    public:
        int getClientSocket()
        {
            return this->clientSocket;
        }
        int getIdServerConnectedTo()
        {
            return this->idServerConnectedTo;
        }
        int size;
        char buffer[65537];
        Client(int sock, int id)
        {
            this->clientSocket = sock;
            fcntl(this->clientSocket, F_SETFL, O_NONBLOCK); // operation of this client socket will be non-blocking the execution of the program will not stop waiting for the client to send data
            this->idServerConnectedTo = id;
            bzero(this->buffer, 65537);
            this->size = 0;
        }
};

class webserv
{
    private: 
        manyServer* servers;
        fd_set read_fds;
        fd_set write_fds;
        fd_set writingSet;
        int server_socket;
        int reuse;
        int max_fds;
        int result;
        std::vector<Socket> sockets;
        std::vector<Client> clients;
    public : 
        webserv(manyServer* servers);
        void proccessClient();
        void createClient();
        void runServer();
        int checkReqErrors(Client client,request req,int size);
        void manageClient();
        int isAllowedTo(Client client, std::string path, std::string method);
        int locationExist(std::string path, Client client);
        void deleteClient(Client client);
        void runMethod(Client client, std::string path,request req);
        void sendError(Client client, int err);
        void setReadSet(int socket, fd_set &set);
        void sendErrorAndRemove(Client client, int error);
     //   int compareSocket(const Socket& a, const Socket& b) {

};

*/
class one_server;
class Webserver;
class Client;
// class Handling;

// class Handling {
//     public:
//         int client_socket;
//         one_server config;
//        //Handling() : client_socket(-1) {}  // Default constructor
//         Handling(one_server serv) : client_socket(-1), config(serv) {}
//         int driver(char *requested_data, int bytesreceived);
//         void setConfig(one_server config);
// };


class Handle{
    public:
        Handle(){};
        int client_socket;
        one_server config;
        Handle(one_server serv) : client_socket(-1), config(serv) {}
        int driver(char *requested_data, int bytesreceived,int active_clt,manyServer* servers,int idx);
        void setConfig(one_server config)
        {
            this->config = config;
        };
};

class Client {
    private:
        int _socket;
    
    public:
        
        Handle _client_handler;
        Client(int socket){
            _client_handler.client_socket = socket;
            this->_socket = socket;
            
        }
        int GetCltSocket() {
            return (this->_socket);
        }
};



class Webserver {
private:
    one_server servers;
    std::list<Client> clients;
    std::list<Client>::iterator itClient;

public:
    Webserver(one_server serv) : servers(serv) {
        std::cout << "listen port : "<< serv.listen << " host: "<< serv._Host << std::endl;
        cl_id = 0;
    }
    int server_socket;
    int client_socket;
    struct addrinfo server_infos;
    struct addrinfo *sinfo_ptr;
    socklen_t client_addr;
    struct sockaddr_storage storage_sock;
    fd_set readfds, writefds, tmpfdread, tmpfdwrite;
    int maxfds;
    int readyforwrite;
    int bytesrev;
    struct timeval          timeout;
    int active_clt;
    char reqData[1024];

    int cl_id;
    void Init();
    void SelectSetsInit();
    void StartServer(manyServer* servers,int i);
    void DeleteClient();
    int AcceptAndAddClientToSet();
    void CreateServer();
    
};



#endif