#include "cgi.hpp"
#include "client_class.hpp"

cgi::cgi()
{
    this->first = true;
    this->env = NULL;
    this->args = new char*[3];
    this->args[0] = NULL;
    this->args[1] = NULL;
    this->args[2] = NULL;
    this->in_fd = -1;
    this->out_fd = -1;
}

cgi::cgi(const cgi& other)
{
    this->env = NULL;
    this->args = NULL;
    this->operator=(other);
}

cgi&    cgi::operator=(const cgi& other)
{
    delete[] this->args;
    delete[] this->env;
    this->env_m.clear();
    this->args = new char*[3];
    this->args[0] = NULL;
    this->args[1] = NULL;
    this->args[2] = NULL;
    this->env = NULL;
    if (other.env)
    {
        this->env = new char*[other.env_m.size() + 1];
        this->env[other.env_m.size()] = NULL;
        for (int i = 0; other.env[i]; i++)
            this->env[i] = strdup(other.env[i]);
    }
    for (int i = 0; other.args[i]; i++)
        this->args[i] = strdup(other.args[i]);
    return (*this);
}

cgi::~cgi()
{
    this->env_m.clear();
    delete[] this->env;
    delete[] this->args;
    close(this->in_fd);
    close(this->out_fd);
}

void cgi::set_cgi(const Location& location, const std::string& path)
{
    std::string extension;
    size_t      point;

    point = path.find_last_of(".");
    if (point == std::string::npos)
        return ;
    extension = path.substr(point);
    this->args[0] = strdup(location.get_cgi(extension).c_str());
}

std::string    create_file_name()
{
    struct timeval  Now;

    gettimeofday(&Now, NULL);
    return (tostring(Now.tv_sec) + "-" + tostring(Now.tv_usec));
}

void    cgi::cgi_init(const one_server& server, const request& req, const std::string& input)
{
    Location    location;
    std::string uri;
    std::string path;
    size_t      find;
    int         idx = 0;

    uri = req.get_path();
    find = uri.find("?");
    path = uri.substr(0, find);
    location = server.get_location(path);
    set_cgi(location, path);
    this->args[1] = strdup(req.get_cgi_script().c_str());
    this->output = "/tmp/" + create_file_name();
    this->out_fd = open(this->output.c_str(), O_RDWR | O_CREAT, 0777);///
    
    if (req.get_method() == "POST")
    {
        struct stat fileStat;

        stat(input.c_str(), &fileStat);
        this->in_fd = open(input.c_str(), O_RDONLY);//freeopen
        this->env_m["CONTENT_TYPE"] = req.get_header("Content-Type");//// get
        this->env_m["CONTENT_LENGTH"] = tostring(fileStat.st_size);
    }
    /////
    
    this->env_m["SERVER_NAME"] = server._server_name;
    this->env_m["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->env_m["SERVER_PROTOCOL"] = HTTP_VERSION;
    this->env_m["SERVER_PORT"] = tostring(server.listen);
    this->env_m["PATH_INFO"] = this->args[1];/////
    this->env_m["REQUEST_METHOD"] = req.get_method();
    this->env_m["SCRIPT_FILENAME"] = args[1];
    if (find != std::string::npos)
        this->env_m["QUERY_STRING"] = uri.substr(find + 1, uri.find("#"));
    ////
    this->env = new char*[this->env_m.size() + 1];
    for (std::map<std::string, std::string>::const_iterator it = this->env_m.cbegin(); it != this->env_m.cend(); it++)
        this->env[idx++] = strdup((it->first + "=" + it->second).c_str());
    this->env[idx] = NULL;
    ////
}

void    cgi::cgi_execute()
{
    float           time;
    struct timeval  Now;

    this->pid = fork();
    if (this->pid == -1)
        this->status = 1;
    else if (!this->pid)
    {
        dup2(this->in_fd, 0);
        dup2(this->out_fd, 1);
        close(this->in_fd);
        close(this->out_fd);
        if (access(this->args[0], X_OK) == 0)
            execve(this->args[0], this->args, this->env);
        exit(1);
    }
    else
    {
        gettimeofday(&Now, NULL);
        while ((time = Now.tv_sec - this->begin.tv_sec + (float)(Now.tv_usec - this->begin.tv_usec) / 1000000) < CGI_TIMEOUT && waitpid(this->pid, &this->status, WNOHANG) != -1)
            gettimeofday(&Now, NULL);
        if ((time = Now.tv_sec - this->begin.tv_sec + (float)(Now.tv_usec - this->begin.tv_usec) / 1000000) >= CGI_TIMEOUT)
        {
            kill(this->pid, SIGKILL);
            this->status = -1;
        }
    }
    close(this->out_fd);
    close(this->in_fd);
}

void cgi::cgi_out(one_server& server, cl& client, const std::string& input)
{
    std::cout<<"CGI CALLED"<<std::endl;
    this->first = false;
    this->cgi_init(server, client.req, input);
    gettimeofday(&this->begin, NULL);
    this->cgi_execute();
    remove(input.c_str());
    std::cout<<"CGI STATUS:"<<this->status<<std::endl;
    if (this->status == -1)
        client.res.set_Status(504, server);
    else if (WEXITSTATUS(this->status) != 0)
    {
        std::cout<<"CGI 500"<<std::endl;
        client.res.set_Status(500, server);
    }
    else
    {
        client.res.set_Status(200, server);
        return ;
    }
    client.end_send = 1;
}

std::string cgi::cgi_run(one_server& server, cl& client, const std::string& input)
{
    std::string line = std::string();

    if (this->first)
    {
        std::cout<<"first"<<std::endl;
        this->cgi_out(server, client, input);
        in.open(this->output, std::ios::in);
        return (client.res.prepare_respons(false));
    }
    else
    {
        std::cout<<"second"<<std::endl;
        // getline(in, line);
        char buffer[BUFFER_SIZE + 1];
        bzero(buffer, BUFFER_SIZE + 1);
        std::cout<<"buffer:"<<buffer<<std::endl;
        in.read(buffer, BUFFER_SIZE);
        if (in.eof())
        {
            in.close();
            client.end_send = 1;
        }
        line = buffer;
    }
    if (client.end_send)
        remove(this->output.c_str());
    return (line);
}