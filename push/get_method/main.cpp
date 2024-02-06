#include "../req_res_post/request.hpp"
#include "../req_res_post/respons.hpp"
#include "../req_res_post/post.hpp"

#define BUFFER_SIZE 1024
// #include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
// #include <cstring>
// #include <fstream>
#include <sys/types.h>
#include <fcntl.h>
#include "../config_file/server.hpp"

int main(int c, char **v)
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        std::cerr << "Error: Socket creation failed\n";
        return 1;
    }
    signal(SIGPIPE, SIG_IGN);
    int reuse = 1;
    int result = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse));
    if ( result < 0 ) {
        perror("ERROR SO_REUSEADDR:");
    }
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;       // IPv4
    server_address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP
    server_address.sin_port = htons(8080);    // Port to listen on (in network byte order)

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        std::cerr << "Error: Binding failed\n";
        return 1;
    }

    if (listen(server_socket, 5) < 0) {
        std::cerr << "Error: Listen failed\n";
        return 1;
    }

    std::cout << "Server is listening on port 8080...\n";

    int client_socket;
    sockaddr_in client_address;
    socklen_t client_addr_size = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_addr_size);
    std::cout << "Connection established with a client\n";
    if (client_socket < 0)
    {
        std::cerr << "Error: Accept failed\n";
        return 1;
    }
    std::cout << "Connected to the server\n";

    char    buffer[2024 + 1];
    size_t  x;
    respons res;
    request var;
    post    p;
    manyServer *server = new manyServer(v[1]);

    std::cout<<"-------------"<<std::endl;
    try
    {
        memset(buffer, 0, 2048 + 1);
        x = read(client_socket, buffer, 2048);
        do
        {
            
            // std::cout<<std::string("").append(buffer, x);
            // z.append(buffer, x);
            if (var.parse_request(std::string("").append(buffer, x)))
            {
                if (var.get_method() == "GET")
                {
                    
                    get_method(var,server, client_socket);}
            else
                std::cout<<"hana"<<std::endl;
            }
        }while (1);
    }
    catch(int status)
    {
        std::cout<<"status : "<<status<<std::endl;
        std::cout<<"-------------"<<std::endl;
        res.set_Status(std::pair<int, std::string>(status, "Bad request"));
        std::cout<<res.prepare_respons(true)<<std::endl;
        write(client_socket, res.prepare_respons(true).c_str(), res.prepare_respons(true).length());
        std::cout<<"-------------"<<std::endl;
    }
    close(client_socket);
    close(server_socket);

    return 0;
}

// class client
// {
// private:
//     int socket_fd;
//     mini_server server;
//     request     req;
//     respons     res;
//     post        p;
//     bool check;
// public:
//     void    get_method();
// };