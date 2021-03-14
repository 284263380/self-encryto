#include "fileUtil.h"
#include <direct.h>

SP_fileUtil_START

std::string getcwd()
{
    char   buffer[MAX_PATH] = {0};
    _getcwd(buffer, MAX_PATH);
    return buffer;
}
std::list<std::string> get_file_list(std::string& path_dir)
{
    std::list<std::string> ret;
    return ret;
}
std::string path_format_linux(std::string path)
{
    for (int i = 0; i < path.length(); i++) {
        if (path.at(i) == '\\') {
            path.at(i) = '/';
        }
    }
    return path;
}
std::string format_dir(std::string path_dir)
{
    int len = path_dir.length();
    if (path_dir.at(len - 1) == '\\' || path_dir.at(len - 1) == '/') {
        path_dir.at(len - 1) = '\0';
    }
    return path_dir;
}


SP_fileUtil_END