#include "webserv.hpp"

std::map<int ,cl>   clients;

int Handle::driver(char *requested_data, int bytesreceived,int active_clt,manyServer* servers,int idx)
{
    if (clients.find(active_clt) == clients.end())
    {
        std::cout<<"CREAT NEW CLIENT::"<<active_clt<<std::endl;
        clients[active_clt] = cl();
    }
    try
    {
        // std::cout<<"000000000::"<<std::string("").append(requested_data, bytesreceived)<<"$"<<std::endl;
        if (clients[active_clt].req.parse_request(std::string("").append(requested_data, bytesreceived)))
        {
            if (clients[active_clt].req.get_method() == "GET")
            {
               // std::cout<<"GET"<<std::endl;
                get_method(clients[active_clt].req, servers, active_clt, idx, clients.find(active_clt)->second);
                if (clients[active_clt].end_send)
                {
                    clients.erase(clients.find(active_clt));
                    return (0);
                }
            }
            else if (clients[active_clt].req.get_method() == "POST")
            {
                if (clients[active_clt].post.post_end)
                    throw (201);
                clients[active_clt].post.post_request(clients[active_clt].req, servers->_name_server[idx]);
                // std::cout<<"DONE"<<std::endl;
            }
            else if (clients[active_clt].req.get_method() == "DELETE")
                delete_request(servers->_name_server[idx], clients[active_clt].req);
            else
            {
                std::cout<<"METOD NOT FOUND"<<std::endl;
                throw (501);
            }
        }
    }
    catch(int status)
    {
        std::string msg;

        std::cout<<"status : "<<status<<std::endl;
        std::cout<<"upload:::::::"<<clients[active_clt].post.get_upload_path() + clients[active_clt].post.get_out_name()<<"$\n";
        clients[active_clt].post.post_end = 1;
        if (status == 201)
        {
            if (clients[active_clt].post.is_cgi)
            {
                std::cout<<"CGI"<<std::endl;
                msg = clients[active_clt].cgi.cgi_run(servers->_name_server[idx], clients[active_clt], clients[active_clt].post.get_upload_path() +"/"+ clients[active_clt].post.get_out_name());
            }
            else
            {
                clients[active_clt].res.set_header(SERVER, servers->_name_server[idx]._server_name);
                clients[active_clt].res.set_post_info(clients[active_clt].post);
                msg = clients[active_clt].res.prepare_respons(true);
                clients[active_clt].end_send = 1;
            }
        }
        else
        {
            clients[active_clt].res.set_Status(status, servers->_name_server[idx]);
            msg = clients[active_clt].res.prepare_respons(true);
            clients[active_clt].end_send = 1;
        }
        std::cout<<"msg::::"<<msg<<"$"<<std::endl;
        write(active_clt, msg.c_str(), msg.length());
        if (clients[active_clt].end_send)
        {
            clients.erase(clients.find(active_clt));
            return (0);
        }
    }
    catch (...)
    {
        std::string msg;
        clients[active_clt].res.set_Status(500, servers->_name_server[idx]);
        msg = clients[active_clt].res.prepare_respons(true);
        std::cout<<"msg:"<<msg<<"$\n";
        write(active_clt, msg.c_str(), msg.length());
        return (0);
    }
    return (1);
}