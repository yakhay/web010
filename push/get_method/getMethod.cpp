/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getMethod.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yakhay <yakhay@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/26 18:40:51 by wbouwach          #+#    #+#             */
/*   Updated: 2024/02/06 23:34:19 by yakhay           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "getMethod.hpp"

bool isDirectory(const std::string& path) {
  struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == 0) {
        return S_ISDIR(fileStat.st_mode);
    }
    return false;
}
std::string determine_content(const std::string& fileExtension)
{
    return (mimetype.get(fileExtension, 1));
}

void send_response_200(const std::string &filename, const std::string &contentType, int newsockfd, cl& client, one_server& server) {
    if (client.test2 == 0)
    {

        client.fileStream.open(filename, std::ios::binary);
        if (!client.fileStream.is_open())
        {
            if (filename == "/tmp/listing.html")
            std::remove("/tmp/listing.html");
            std::cout <<"hhhh7\n";
            throw (403);
        }

        struct stat statbuf;
        if (stat(filename.c_str(), &statbuf) == -1)
        {
            if (filename == "/tmp/listing.html")
            std::remove("/tmp/listing.html");
            std::cout <<"hhhh0\n";
            throw (404);
        }

        size_t size = statbuf.st_size;
        std::cout << "path: " << filename << std::endl;

        client.res.set_header("Content-Length", std::to_string(size));
        client.res.set_header("Content-type", contentType);
        std::cerr << "Type : " << contentType << std::endl;
        client.res.set_Status(200, server);
        std::string response = client.res.prepare_respons(true);
        std::cout << response << std::endl;
        int dd = send(newsockfd, response.c_str(), response.size(), 0);

        if (dd < 0) {
            perror("webserver send failed");
            if (filename == "/tmp/listing.html")
            std::remove("/tmp/listing.html");
            client.end_send = 1;
            client.fileStream.close();
            return;
        }
        client.test2 = 1;
    }
    else
    {
        client.fileStream.read(client.buffer.data(), client.buffer_size);
        if (client.fileStream.gcount() < 0)
        {
            perror("webserver read failed");
            if (filename == "/tmp/listing.html")
            std::remove("/tmp/listing.html");
            client.end_send = 1;
            client.fileStream.close();
            return;
        }
        // std::cout << client.fileStream.gcount() << std::endl;
        int valwrite = send(newsockfd, client.buffer.data(), client.fileStream.gcount(), 0);
        if (valwrite < 0) {
            perror("webserver send failed");
            if (filename == "/tmp/listing.html")
            std::remove("/tmp/listing.html");
            client.end_send = 1;
            client.fileStream.close();
            return;
        }
    }
    if (client.fileStream.eof())
    {
        client.end_send = 1;
        if (filename == "/tmp/listing.html")
            std::remove("/tmp/listing.html");
    }
}

void get_method(request &req, manyServer *server, int newsockfd, int idx, cl& client) {
    std::string new_path;
    std::string contentType;
    if (client.lock == 0)
    {    
        client.lock = 1;
        client.location = server->_name_server[idx]._location.begin();
        while (client.location != server->_name_server[idx]._location.end())
        {
            if (req.get_path().find(client.location->_name_Location) == 0)
            {
                
                client.path = client.location->_root + req.get_path().substr(client.location->_name_Location.size(),req.get_path().size() - client.location->_name_Location.size());
                break;
            }
            client.location++;
        }
        if (client.location == server->_name_server[idx]._location.end())
            {
                // exit(0);
                std::cout <<"hhhh3\n";
                throw (404);
            }
        if (find(client.location->_allow_methods.cbegin(), client.location->_allow_methods.cend(), "GET") == client.location->_allow_methods.cend())
            throw (405);
        size_t find = req.get_path().find_last_of("/");  
        if (find != std::string::npos && (req.get_path().c_str())[find + 1] == '\0')
        {
            std::string str_ret = req.get_path().substr(0,req.get_path().size() - 1);
            std::string response = "HTTP/1.1 301 Moved Permanently\r\n";
            response += "Location: "+  str_ret +"\r\n\r\n";
            int dd = send(newsockfd, response.c_str(), response.size(), 0);
            close(newsockfd);
            client.end_send = 1;
            return ;
        }
        if (access(client.path.c_str(), R_OK ) != 0)
        {
            if (access(client.path.c_str(), R_OK | X_OK) != 0)
            {
                // exit(0);
                std::cout <<"hhhh1\n";
                throw (404);
                }
        }
    }
    new_path = client.path;
    if (client.location->_return)
    {
        std::string str_ret(client.location->_return , strlen(client.location->_return));
        std::string response = "HTTP/1.1 301 Moved Permanently\r\n";
        response += "Location: "+  str_ret +"\r\n\r\n";
        int dd = send(newsockfd, response.c_str(), response.size(), 0);
        close(newsockfd);
        client.end_send = 1;
        return ;
    }
    else if (!isDirectory(new_path) ) {
            if ((new_path.find_last_of('.') != std::string::npos) && !(client.location->get_cgi(new_path.substr(new_path.find_last_of('.'))).empty()))
            {
                std::cout << "lah akbar : " << new_path << std::endl;
                std::cout<<"RUN CGI FROM GET2"<<std::endl;
                std::string msg;
            
                client.req.set_cgi_script(new_path);
                msg = client.cgi.cgi_run(server->_name_server[idx], client, "");
                write(newsockfd, msg.c_str(), msg.length());
            }
            else
            {
                if (new_path.find_last_of(".") == std::string::npos)
                contentType = "application/octet-stream";
                else
                contentType = determine_content(new_path.substr(new_path.find_last_of(".") + 1));
                send_response_200(new_path, contentType, newsockfd, client, server->_name_server[idx]);
            }
         }
        else if (client.location->index) {
            std::string index = (client.location->index);
            new_path = new_path + "/" + index;
                if (client.location->cgi_status == 1)
                {
                    if ((new_path.find_last_of('.') != std::string::npos) && !(client.location->get_cgi(new_path.substr(new_path.find_last_of('.'))).empty()))
                {
                    std::string msg;
                
                    client.req.set_cgi_script(new_path);
                    msg = client.cgi.cgi_run(server->_name_server[idx], client, "");
                    write(newsockfd, msg.c_str(), msg.length());
                }
                else
                {
                    if (new_path.find_last_of(".") == std::string::npos)
                        contentType = "application/octet-stream";
                    else
                    contentType = determine_content(new_path.substr(new_path.find_last_of(".") + 1));
                    send_response_200(new_path, contentType, newsockfd, client, server->_name_server[idx]);
                }
                return ;
            } else {
                if (new_path.find_last_of(".") == std::string::npos)
                    contentType = "application/octet-stream";
                else
                contentType = determine_content(index.substr(index.find_last_of(".") + 1));
                send_response_200(new_path +"/" + index, contentType, newsockfd, client, server->_name_server[idx]);
        }
    }
    else if (client.location->_autoindex == 1) {
        if (client.test3 == 0) {
            std::remove("/tmp/listing.html");
            std::fstream outputFile("/tmp/listing.html", std::ios::in | std::ios::out | std::ios::app);
            outputFile.seekp(0, std::ios::beg);
            outputFile << "";
            if (!outputFile.is_open()) {
                std::cerr << "Error opening the file: " << std::endl;
                return;
            }
            client.test3 = 1;

            std::string buffer = "<html><head>\n<title>Directory Listing</title></head><body>\n<h1>Directory Listing</h1><ul>\n";
            DIR *dir = opendir(new_path.c_str());

            if (dir == nullptr) {
                std::cerr << "Error opening directory." << std::endl;
                return;
            }
            struct dirent *entry;
            while ((entry = readdir(dir)) != nullptr) {
                std::string filename = entry->d_name;
                if (entry->d_type == DT_REG) {
                    
                    if (buffer.find(entry->d_name) == std::string::npos)
                        buffer = buffer + "<li>file :  <a href=\"" + req.get_path() + "/" + filename + "\">" + filename + "</a></li> \n";
                }
                     else if (entry->d_type == DT_DIR && std::string(entry->d_name) != "." && std::string(entry->d_name) != "..") {
                            buffer += "<li> dir : <a href=\"" + req.get_path() + "/" + filename + "\">" + filename + "</a></li>\n";
                    
                }
            }
            outputFile << buffer + "</ul></body></html>\r\n";
            outputFile.close();
            closedir(dir);
            contentType = "text/html"; 
        }
        
        send_response_200("/tmp/listing.html", contentType, newsockfd, client, server->_name_server[idx]);
    } 
    else if (client.location->_autoindex == 0){
         throw(403);
    }
}
