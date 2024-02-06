#include <sys/stat.h>
#include <cstring>
#include <iostream>

bool isDirectory(const std::string& path) {
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == 0) {
        return S_ISDIR(fileStat.st_mode);
    }
    return false;
}


int main(int argc, char const *argv[])
{
    std::cout << isDirectory("/Users/yakhay/Desktop/yame/Media/feed/") << std::endl;
    return 0;
}
