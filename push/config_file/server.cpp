/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yakhay <yakhay@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 18:06:35 by wbouwach          #+#    #+#             */
/*   Updated: 2024/01/07 15:54:24 by yakhay           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"


// void extract_request_info(const std::string& buffer, std::string& method, std::string& version, std::string& url) {
//     method = buffer.substr(0, buffer.find(' '));
//     version = buffer.substr(buffer.find(' ', buffer.find(' ') + 1) + 1,
//                             buffer.find('\r') - buffer.find(' ', buffer.find(' ') + 1) - 1);
//     url = buffer.substr(buffer.find(' ') + 1,
//                         buffer.find(' ', buffer.find(' ') + 1) - buffer.find(' ') - 1);
// }

// // Function to construct the file path based on the requested URL
// static std::string construct_file_path(const std::string& url, manyServer* server) {
//     std::size_t firstSlash = url.find('/');
//     std::string tmpurl = (firstSlash != std::string::npos) ? url.substr(firstSlash + 1) : url;
//     std::size_t secondSlash = tmpurl.find('/');
//     std::string newUrl = (secondSlash != std::string::npos) ? tmpurl.substr(secondSlash) : tmpurl;

//     std::string pathFile = server->_name_server[0]._location[0]._root;
//     for (int i = 0; i < 4; i++) {
//         std::string locationtoFind = url.substr(1);
//         locationtoFind = locationtoFind.substr(0, locationtoFind.find('/'));
//         if (locationtoFind == server->_name_server[0]._location[i]._name_Location.substr(1)) {
//             std::cout << "--here--" << std::endl;
//             pathFile = server->_name_server[0]._location[i]._root;
//             break;
//         }
//     }

//     pathFile += newUrl;
//     return pathFile;
// }

// // Function to send a 404 Not Found response
// void send_not_found_response(int newsockfd) {
//     char resp[] = "HTTP/1.0 404 Not Found\r\n"
//                   "Server: webserver-c\r\n"
//                   "Content-Length: 25\r\n"
//                   "Content-type: text/html\r\n\r\n"
//                   "<html>404 Not Found</html>\r\n";
//     int valwrite = send(newsockfd, resp, strlen(resp), 0);

//     if (valwrite < 0) {
//         perror("webserver (write) failed");
//     }
// }

// // Function to determine the content type based on the file extension
// std::string determine_content_type(const std::string& fileExtension) {
//     if (fileExtension == "html") {
//         return "text/html";
//     } else if (fileExtension == "pdf") {
//         return "application/pdf";
//     } else if (fileExtension == "mp4") {
//         return "video/mp4";
//     } else if (fileExtension == "png") {
//         return "image/png";
//     } else {
//         return "application/octet-stream";
//     }
// }

// // Function to handle serving the file content
// void serve_file_content(int newsockfd, const std::string& pathFile, int size, const std::string& contentType) {
//     std::ifstream fileStream(pathFile, std::ios::in | std::ios::binary);
//     if (fileStream.is_open()) {
//         std::cout << "File opened: " << std::endl;
//         // Construct the HTTP response headers
//         std::string response_headers = "HTTP/1.0 200 OK\r\n"
//                                       "Server: webserver-c\r\n"
//                                       "Content-Length: " + std::to_string(size) + "\r\n"
//                                       "Content-type: " + contentType + "\r\n"
//                                       "Cache-Control: no-cache, no-store, must-revalidate\r\n"
//                                       "Pragma: no-cache\r\n"
//                                       "Expires: 0\r\n\r\n";

//         // Send the headers
//         int valwrite = send(newsockfd, response_headers.c_str(), response_headers.length(), 0);

//         if (valwrite < 0) {
//             perror("webserver (write) failed");
//             fileStream.close();
//             return;
//         }

//         // Read and send the file content dynamically
//         const int buffer_size = 4096;  // Adjust the buffer size to a larger value
//         std::vector<char> buffer(buffer_size);

//         while (fileStream.read(buffer.data(), buffer_size), fileStream.gcount() > 0) {
//             int bytesRead = fileStream.gcount();
        
//             // Send the chunk
//             valwrite = send(newsockfd, buffer.data(), bytesRead, 0);
//             std::cout << "i send " << bytesRead << " bytes\n";
//             if (valwrite < 0) {
//                 perror("webserver (write) failed");
//                 break;
//             } 
        
//             // Clear the buffer before the next read
//             buffer.clear();
//             buffer.resize(buffer_size);
        
//             //add a delay
//             //usleep(200);
//         }




//         // Close the file stream
//         fileStream.close();

//         std::cout << "File served: " << pathFile << std::endl;
//     } else {
//         std::cerr << "Unable to open file: " << pathFile << std::endl;
//         send_not_found_response(newsockfd);
//     }
// }


// // Function to handle the main request parsing and processing logic
// void process_request(std::string buffer, int newsockfd, manyServer* server) {
//     std::string method, version, url;
//     extract_request_info(buffer, method, version, url);

//     std::string pathFile = construct_file_path(url, server);

//     struct stat statbuf;
//     if (stat(pathFile.c_str(), &statbuf) == -1) {
//         std::cout << "File noooot found" << std::endl;
//         send_not_found_response(newsockfd);
//     } else {
//         int size = statbuf.st_size;
//         std::cout << "File size: " << size << std::endl;
//         std::string fileExtension;
//         std::cout << "pathfile : " << pathFile<< std::endl;
//         size_t dotPosition = pathFile.find_last_of(".");
//         if (dotPosition != std::string::npos) {
//             fileExtension = pathFile.substr(dotPosition + 1);
//         }
//         std::cout << "File extension: " << fileExtension << std::endl;
//         std::string contentType = determine_content_type(fileExtension);
//         std::cout << "Content-type: " << contentType << std::endl;
//         serve_file_content(newsockfd, pathFile, size, contentType);
//     }
// }



int main(int ac,char **av)
{
    
    if (ac != 2)
    {
        std::cout << "Error: wrong number of arguments" << std::endl;
        return 1;
    }
    manyServer *server = new manyServer(av[1]);
    request req;
    respons res;
   
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    char buffer[BUFFER_SIZE];
    if (sockfd < 0)
    {
        perror("socket failed");
        return 1;
    }
    int reuse = 1;
    int result = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse));
    if ( result < 0 ) {
        perror("ERROR SO_REUSEADDR:");
    }
    
    int host_addr_len = sizeof(serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server->_name_server[0].listen);
    serv_addr.sin_addr.s_addr = inet_addr(server->_name_server[0]._Host.c_str());
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
    {
        perror("bind failed\n");
        return 1;
    }
    try{
        
        while (1)
        {
            if (listen(sockfd, 10) != 0)
            {
                perror("listen failed\n");
                return 1;
            }
            int newsockfd = accept(sockfd, (struct sockaddr *)&serv_addr,
                                    (socklen_t *)&host_addr_len);
            if (newsockfd < 0) {
                perror("webserver (accept) failed\n");
            }
            std::cout<<"connection accepted\n";

            // Read from the socket
            std::string request_data;
            while (1) {
                int valread = recv(newsockfd, buffer, sizeof(buffer), 0);
                if (valread < 0) {
                    perror("webserver (read) failed\n");
                    break;
                } else if (valread == 0) {
                    // Connection closed by the client
                    break;
                }
    
                // Append the received data to the request_data string
                request_data.append(buffer, valread);
    
                // Check if we have received the entire request
                if (request_data.find("\r\n\r\n") != std::string::npos) {
                    break; // Received the end of the request
                }
            }

            request_data.push_back('\0'); // Add a null-terminator to the end of the string
            
            if (req.parse_request(request_data))
            { 
                if (req.get_method() == "GET")
                {
                    
                    std::cout << "Handleling GET request" << std::endl;
                    //process_request(request_data,newsockfd,server);
                    // while (1)
                    // {
                       //std::cout<<"((GET))"<<std::endl;
                        get_method(req,server,newsockfd);
                        //std::cout<<"((GET end))"<<std::endl;
                    // }
                }
                else 
                    return 0;
            }
            std::cout << "i will close the socket" << std::endl;
            close(newsockfd);
        }
    }catch(int e)
    {
        std::cout<<"error : "<<e<<std::endl;
    }
    
    return 0;
}