#include "delete.hpp"
#include <algorithm>

int status;

void    parse_path(std::string path)
{
    struct stat path_Stat;

    if (path.back() == '/')
        path.pop_back();
    path = path.substr(0, path.find('/'));
    while (!path.empty())
    {
        stat(path.c_str(), &path_Stat);
        if (!(S_ISDIR(path_Stat.st_mode) && (path_Stat.st_mode & S_IWOTH)) || !(S_ISDIR(path_Stat.st_mode) && (path_Stat.st_mode & S_IXOTH)))
            throw (403);
        path.pop_back();
        path = path.substr(0, path.find_last_of('/'));
    }
}

void    del_dir(std::string path)
{
    struct stat path_Stat;

    stat(path.c_str(), &path_Stat);
    if (S_ISDIR(path_Stat.st_mode))
    {
        DIR* dir;

        dir = opendir(path.c_str());
        if (!dir)
        {
            status = 500;
            return ;
        }
        struct dirent* entry;
        while ((entry = readdir(dir)))
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                if ((path_Stat.st_mode & S_IROTH) && (path_Stat.st_mode & S_IWOTH) && (path_Stat.st_mode & S_IXOTH)
                    && (path_Stat.st_mode & S_IRUSR) && (path_Stat.st_mode & S_IWUSR) && (path_Stat.st_mode & S_IXUSR))
                    del_dir(path + "/" + entry->d_name);
                else
                {
                    status = 500;
                    closedir(dir);
                    return ;
                }
            }
        }
        if (path_Stat.st_mode & S_IROTH)
            rmdir(path.c_str());
        else
            status = 500;
        closedir(dir);
    }
    else if (S_ISREG(path_Stat.st_mode))
        remove(path.c_str());
    else
        status = 500;
}

void    delete_request(const one_server& server, const request& req)
{
    struct stat path_Stat;
    Location    location;
    std::string url;
    std::string cpath;

    status = 204;
    url = req.get_path();
    location = server.get_location(url.substr(0, url.find("?")));
    if (find(location._allow_methods.cbegin(), location._allow_methods.cend(), "DELETE") == location._allow_methods.cend())
        throw (405);
    cpath = server.get_path(url.substr(0, url.find("?")));
    // parse_path(prev_dir(path));
    stat(cpath.c_str(), &path_Stat);
    if (S_ISDIR(path_Stat.st_mode))
    {
        if (cpath.back() != '/')
            throw (409);
        del_dir(cpath);
    }
    else if (S_ISREG(path_Stat.st_mode))
        remove(cpath.c_str());
    else
        throw (404);
    throw (status);
}

    // location /post{
    //     root : /Users/yakhay/Desktop/yakh/Media/feed;
    //     allow_methods : POST GET DELETE;
    //     autoindex : of;
    //     upload : on;
    //     cgi_status : of;
    //     upload_path : /Users/yakhay/Desktop/post/;
    //     cgi_path .py :/usr/bin/python3;
    //     cgi_path .php : /usr/bin/php;
    //     cgi_path .pl : /usr/bin/perl;
    //     cgi_path .sh : /bin/bash;
    // }