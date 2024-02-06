#include "post.hpp"
#include "MimeType.hpp"
#include <algorithm>

std::string tostring(size_t n)
{
    char        num[1024];

    std::sprintf(num, "%lu", n);
    return (num);
}

post::post()
{
    this->post_end = false;
    this->upload_path = "/tmp/";
    this->mode = 0;
    this->chunked_size = 0;
    this->first_time = true;
    this->h = false;
    this->is_cgi = false;
}

std::string post::get_out_name() const
{
    return (this->out_name);
}

std::string post::get_upload_path() const
{
    return (this->upload_path);
}

std::string post::get_url() const
{
    return (this->url);
}

void    post::create_file(std::string content_type)
{
    std::string     extension;
    struct timeval  Now;

    extension = mimetype.get(content_type, 0);
    gettimeofday(&Now, NULL);
    this->out_name = tostring(Now.tv_sec) + "-" + tostring(Now.tv_usec) + "." + extension;
    this->out.open(this->upload_path + this->out_name, std::ios::out);
}

std::string post::set_headers(std::string body)
{
    std::string line;
    size_t      pos;
    size_t      hed;

    while (this->h && (pos = body.find(LINE_SEPARATOR)) != std::string::npos)
    {
        line = body.substr(0, pos);
        body = body.substr(pos + strlen(LINE_SEPARATOR));
        hed = line.find(HEDER_SEPARATOR);
        if (line.empty())
        {
            this->h = false;
            this->out.close();
            this->create_file(this->headers[CONTENT_TYPE]);
            break;
        }
        if (hed == std::string::npos)
            throw (400);
        this->headers[line.substr(0, hed)] = line.substr(hed + strlen(HEDER_SEPARATOR));
    }
    return (body);
}

void    post::boundarry(std::string body)
{
    std::string start = "--" + this->boundary + "\r\n";
    std::string last = "--" + this->boundary + "--" + "\r\n";
    std::string tmp;
    size_t      exist;

    body = this->brest + body;
    while ((exist = body.find(start)) != std::string::npos)
    {
        tmp = this->set_headers(body.substr(0, exist));
        body = body.substr(exist + start.length());
        if (this->out.is_open())
            this->out<<tmp.substr(0, tmp.length() - strlen(LINE_SEPARATOR));
        this->h = true;
    }
    if ((exist = body.find(last)) != std::string::npos)
    {
        body = this->set_headers(body.substr(0, exist));
        if (this->out.is_open())
            this->out<<body.substr(0, body.length() - strlen(LINE_SEPARATOR));
        this->out.close();
        throw (201);
    }
    body = this->set_headers(body);
    if (this->h)
    {
        this->brest = body;
        return ;
    }
    if (body.length() <= start.length())
    {
        this->brest = body;
        return ;
    }
    this->brest = body.substr(body.length() - start.length());
    this->out<<body.substr(0, body.length() - start.length());
}

int hexCharToInt(char hexChar)
{
    if (hexChar >= '0' && hexChar <= '9')
        return (hexChar - '0');
    else if (hexChar >= 'A' && hexChar <= 'F')
        return (hexChar - 'A' + 10);
    else if (hexChar >= 'a' && hexChar <= 'f')
        return (hexChar - 'a' + 10);
    throw (400); //bad hexdecimal number;
}

size_t  hex_to_dec(std::string n)
{
    size_t  result;

    result = 0;
    for (size_t i = 0; i < n.length(); ++i)
        result = result * 16 + hexCharToInt(n[i]);
    return result;
}

void    post::chunked(std::string body)
{
    size_t      hex_end;
    std::string n;
    std::string tmp;
    tmp = body;
    this->rest += body;
    body = "";
    if (this->chunked_size == 0)
    {
        hex_end = this->rest.find(LINE_SEPARATOR);
        if (hex_end == std::string::npos) //"\r\n" not found
        {
            if (this->rest.length() >= 16) // hexdecemal number too long
                throw (400);
            return ;
        }
        n = this->rest.substr(0, hex_end); // parse n important
        if (n.empty())
        {
            this->rest = this->rest.substr(hex_end + strlen(LINE_SEPARATOR));
            this->chunked("");
            return ;
        }
        this->chunked_size = hex_to_dec(n);
        if (this->chunked_size == 0)
        {
            this->out.close();
            throw (201);
        }
        this->rest = this->rest.substr(hex_end + strlen(LINE_SEPARATOR));
    }
    if (this->chunked_size >= this->rest.length()/*important ==>'\0' inside buffer*/)
    {
        if (this->mode == 1 && this->is_cgi)
            this->raw(this->rest);
        else if (this->mode == 3)
            this->boundarry(this->rest);
        else
            this->out<<this->rest;
        this->chunked_size -= this->rest.length();
        this->rest = "";
    }
    else if (this->chunked_size)
    {
        if (this->mode == 1 && this->is_cgi)
            this->raw(this->rest.substr(0, this->chunked_size));
        else if (this->mode == 3)
            this->boundarry(this->rest.substr(0, this->chunked_size));
        else
            this->out<<this->rest.substr(0, this->chunked_size);
        this->rest = this->rest.substr(this->chunked_size);
        this->chunked_size = 0;
        this->chunked(body);
    }
}

void    post::raw(std::string content)
{
    if (this->mode == 4)
    {
        size_t  pos;
        content = this->brest + content;
        if ((pos = content.find("--" + this->boundary + "--" + "\r\n")) != std::string::npos)
        {
            this->out<<content;//.substr(0, pos);
            this->out.close();
            throw (201);
        }
        else
        {
            if (content.length() > ("--" + this->boundary + "--" + "\r\n").length())
            {
                this->out<<content.substr(0, content.length() - ("--" + this->boundary + "--" + "\r\n").length());
                this->brest = content.substr(content.length() - ("--" + this->boundary + "--" + "\r\n").length());
            }
            else
                this->brest += content;
        }
    }
    else
    {
        if (this->max_size < content.length())
            content = content.substr(0, this->max_size);
        this->out<<content;
        this->max_size -= content.length();
        if (this->max_size == 0)
        {
            this->out.close();
            throw (201);
        }
    }
}

void    post::parse_uri(request& req, const one_server& server)
{
    Location        location;
    struct stat     path_info;
    struct stat     content_info;
    DIR             *dir;
    struct dirent   *entry;
    std::string     extension;
    std::string     path;
    size_t          exist;

    location = server.get_location(this->url);
    if (find(location._allow_methods.cbegin(), location._allow_methods.cend(), "POST") == location._allow_methods.cend())
        throw (405);
    path = server.get_path(this->url);
    stat(path.c_str(), &path_info);
    if (location.cgi_status)
    {
        std::cout<<"1"<<std::endl;
        if (S_ISREG(path_info.st_mode))
        {
            std::cout<<"2"<<std::endl;
            exist = path.find_last_of(".");
            if (exist != std::string::npos)
                extension = path.substr(exist);
            if (!location.get_cgi(extension).empty())
            {
                this->is_cgi = true;
                req.set_cgi_script(path);
                std::cout<<"3"<<extension<<std::endl;
                return ;
            }
        }
        else if (S_ISDIR(path_info.st_mode))
        {
            std::cout<<"4"<<std::endl;
            dir = opendir(path.c_str());
            while (dir && location._autoindex && (entry = readdir(dir)))
            {
                stat((path + "/" + entry->d_name).c_str(), &content_info);
                if (S_ISREG(path_info.st_mode))
                {
                    exist = (path + "/" + entry->d_name).find_last_of(".");
                    if (exist != std::string::npos)
                        extension = (path + "/" + entry->d_name).substr(exist);
                    if (location.is_index(entry->d_name) && !location.get_cgi(extension).empty())
                    {
                        std::cout<<"4"<<std::endl;
                        req.set_cgi_script(path + "/" + entry->d_name);
                        this->is_cgi = true;
                        closedir(dir);
                        return ;
                    }
                }
            }
            closedir(dir);
        }
    }
    std::cout<<"5"<<std::endl;
    if (!location._upload)
        throw (403);
    std::cout<<"6"<<std::endl;
    this->upload_path = std::string(location.upload_path) + "/";
}

void    post::init(request& req,  one_server& server)
{
    this->first_time = false;
    //url
    this->url = req.get_path().substr(0, req.get_path().find("?"));
    this->parse_uri(req, server);
    //headers
    this->content_type = req.get_header(CONTENT_TYPE);
    this->content_length = req.get_header(CONTENT_LENGTH);
    if (this->content_type.empty())
        throw (400);
    this->transfer_encoding = req.get_header(TRANSFER_ENCODING);
    if (this->content_type.substr(0, this->content_type.find(";")) == MULTIPART)
        this->boundary = this->content_type.substr(this->content_type.find("boundary=") + strlen("boundary="));
    //location
    //mode
    //chunked
    std::cout<<"content:"<<this->content_type<<"$\n";
    if (!this->transfer_encoding.empty()) //if it exist but empty
    {
        std::cout<<"CHUNCKED"<<std::endl;
        if (!this->content_length.empty())//if it exist but empty
            throw (400);
        if (this->transfer_encoding != "chunked")
            throw (405); //not allowed
        this->mode = 1;
        if (this->boundary.empty())
            this->create_file(this->content_type);
        else if (this->is_cgi)
        {
            this->out_name = create_file_name();////
            this->out.open(this->upload_path + this->out_name, std::ios::out);
        }
        else
            this->mode = 3;
        return ;
    }
    //boundary
    if (!this->boundary.empty())
    {
        std::cout<<"BOUNDARY"<<std::endl;
        if (!this->content_length.empty())
            throw (400);
        this->mode = 2;
        if (!is_cgi)
            return ;
        this->mode = 4;
        this->out_name = create_file_name();
        this->out.open(this->upload_path + this->out_name, std::ios::out);
        return ;
    }
    //raw
    std::cout<<"RAWWW"<<std::endl;
    if (this->content_length.empty())
        throw (411);
    this->create_file(this->content_type);
    this->max_size = atoi(this->content_length.c_str());
}

void    post::post_request(request& req, one_server& server)
{
    std::string content;

    std::cout<<"-------------------------"<<std::endl;
    content = req.get_body();
    if (this->first_time)
        this->init(req, server);
    if (this->mode == 1 || this->mode == 3)
        this->chunked(content);
    else if (this->mode == 2)
        this->boundarry(content);
    else
        this->raw(content);
    std::cout<<"-------------------------"<<std::endl;
}