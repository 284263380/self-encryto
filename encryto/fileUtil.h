#ifndef __FILEUTIL_H__
#define __FILEUTIL_H__

#include<list>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <windows.h>
using namespace std;

#define SP_fileUtil_START namespace fileUtil {
#define SP_fileUtil_END }
SP_fileUtil_START

//获取当前文件夹
std::string getcwd();
//获取文件夹的所有文件
std::list<std::string> get_file_list(std::string& path_dir);
std::string path_format_linux(std::string path);


SP_fileUtil_END



#endif//__FILEUTIL_H__
