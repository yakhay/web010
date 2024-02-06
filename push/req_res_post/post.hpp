#ifndef POST_HPP
#define POST_HPP

# include <sys/time.h>
# include <dirent.h>
# include <sys/stat.h>
#include "../config_file/configFile.hpp"
#include "request.hpp"
#include "cgi.hpp"

class MimeType;

extern  MimeType mimetype;

class post
{
private:
    std::fstream    out;
    bool            first_time;
    size_t          max_size;
    size_t          chunked_size;
    //
    std::string     boundary;
    std::string     brest;
    std::map<std::string, std::string> headers;
    bool            h;
    //
    void            create_file(std::string extension);
    //
    std::string     content_type;
    std::string     content_length;
    std::string     transfer_encoding;
    ///
    short           mode;
    std::string     out_name;
    std::string     url;
    std::string     upload_path;
    std::string     rest;
public:
    bool            post_end;
    bool            is_cgi;
    post();
    void        parse_uri(request& req, const one_server& server);
    void        init(request& req, one_server& server);
    void        chunked(std::string body);
    void        boundarry(std::string body);
    void        raw(std::string content);
    void        post_request(request& req, one_server& server);
    std::string get_out_name() const;
    std::string get_upload_path() const;
    std::string get_url() const;
    std::string set_headers(std::string b);
    post&   operator=(const post& other)
    {
        this->upload_path = other.upload_path;
        this->mode = other.mode;
        this->chunked_size = other.chunked_size;
        this->first_time = other.first_time;
        this->h = other.h;
        this->is_cgi = other.is_cgi;
        return (*this);
    }
};

std::string tostring(size_t n);

#endif