#define CRYPTOPP_DEFAULT_NO_DLL
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <direct.h>
#include <io.h>
#include <windows.h>
#include "dll.h"
#include "cryptlib.h"
#include "aes.h"
#include "md5.h"
#include <limits.h>
#include <stdarg.h>
using namespace std;
using namespace CryptoPP;

#pragma warning(disable:4996)
//*******************************************************
static char s_buffC[16] = { 0 };
static bool del = 0;
//*******************************************************
SecByteBlock HexDecodeString(const char *hex)
{
    StringSource ss(hex, true, new HexDecoder);
    SecByteBlock result((size_t)ss.MaxRetrievable());
    ss.Get(result, result.size());
    return result;
}
void AES_CTR_Encrypt2(const char *hexKey, const char *hexIV, const char *infile, const char *outfile)
{
    SecByteBlock key = HexDecodeString(hexKey);
    SecByteBlock iv = HexDecodeString(hexIV);
    CTR_Mode<AES>::Encryption aes(key, key.size(), iv);
    FileSource(infile, true, new StreamTransformationFilter(aes, new FileSink(outfile)));
}
void AES_CTR_Encrypt(const char *hexKey, const char *hexIV, const char *infile, const char *outfile)
{
    CTR_Mode<AES>::Encryption aes((const byte*)hexKey, AES::DEFAULT_KEYLENGTH, (const byte*)hexIV);
    FileSource(infile, true, new StreamTransformationFilter(aes, new FileSink(outfile)));
}
void AES_CTR_Decrypt(const char *hexKey, const char *hexIV, const char *infile, const char *outfile)
{
    CTR_Mode<AES>::Decryption aes((const byte*)hexKey, AES::DEFAULT_KEYLENGTH, (const byte*)hexIV);
    FileSource(infile, true, new StreamTransformationFilter(aes, new FileSink(outfile)));
}
void MD5Buff16(char* buff)
{
    Weak::MD5 md5;
    md5.Update((unsigned char*)buff, strlen(buff));
    md5.Final((unsigned char*)s_buffC);
}
std::string MD5File(std::string file)
{
    CryptoPP::Weak1::MD5 md;
    const size_t size = CryptoPP::Weak1::MD5::DIGESTSIZE * 2;
    byte buf[size] = { 0 };
    CryptoPP::FileSource(file.c_str(), true,new CryptoPP::HashFilter(md,new CryptoPP::HexEncoder(new CryptoPP::ArraySink(buf, size))));
    string strHash = string(reinterpret_cast<const char*>(buf), size);
    return strHash;
}
std::string pathGetFileName(std::string& fullPath)
{
    if (fullPath.empty())
        return fullPath;
    int pos = fullPath.find_last_of("/\\");
    std::string path = fullPath.substr(pos + 1);
    pos = path.find_last_of('.');
    return path.substr(0, pos);
}
std::string pathGetFileNameWithExt(std::string& fullPath)
{
    if (fullPath.empty())
        return fullPath;
    int pos = fullPath.find_last_of("/\\");
    return fullPath.substr(pos + 1);
}
std::string pathGetFileNameExt(std::string& fullPath)
{
    std::string str = pathGetFileNameWithExt(fullPath);
    int pos = str.find_last_of(".");
    if (pos + 1>int(str.length() - 1))
        return "";
    else
        return fullPath.substr(pos + 1);
}
std::string pathGetFilePath(std::string& fullPath)
{
    if (fullPath.empty())
        return fullPath;
    int pos = fullPath.find_last_of("/\\");
    return fullPath.substr(0, pos);
}
//*******************************************************
void doFile(std::string path, const std::function<bool(std::string file)> &func)//bool (*func)(std::string file,std::string v)
{
    std::string path2 = path;
    path2 += "/*.*";
    _finddata_t fileinfo;
    int hFile = _findfirst(path2.c_str(), &fileinfo);
    if (hFile == -1)
        return;
    do
    {
        if (!(fileinfo.attrib & _A_SUBDIR))
        {
            if (!func(fileinfo.name))
                continue;
        }
    } while (_findnext(hFile, &fileinfo) == 0);
    _findclose(hFile);
}
bool encodeFilter(string& file)
{
    static string current_dir = "";
    if (current_dir.empty()) {
        char buffer[MAX_PATH] = { 0 };
        getcwd(buffer, MAX_PATH);
        current_dir = buffer;
    }
    std::string ext = pathGetFileNameExt(file);
    if(ext!="exe" && ext != "py" && ext != "bat" && ext.at(ext.length()-1)!='_'){
        std::string pathCur1 = current_dir;
        std::string path_src = pathCur1 + "/" + file;
        std::string stdMd5 = MD5File(path_src);
        transform(stdMd5.begin(), stdMd5.end(), stdMd5.begin(), ::tolower);
        std::string path_dest = pathCur1 + "/" + stdMd5 +"."+ext;
        printf("%s=>%s\n", stdMd5.c_str(),file.c_str());
        AES_CTR_Encrypt(s_buffC, s_buffC, path_src.c_str(), std::string(path_dest + "_").c_str());
        remove(path_src.c_str());
    }
    return true;
}
bool decodeFilter(string& file)
{
    static string current_dir = "";
    if (current_dir.empty()) {
        char buffer[MAX_PATH] = { 0 };
        getcwd(buffer, MAX_PATH);
        current_dir = buffer;
    }
    std::string ext = pathGetFileNameExt(file);
    if(ext.at(ext.length() - 1) == '_'){
        std::string pathCur1 = current_dir;
        pathCur1 += "\\";
        pathCur1 += file;
        printf("%s\n", file.c_str());
        AES_CTR_Decrypt(s_buffC, s_buffC, pathCur1.c_str(), pathCur1.substr(0, pathCur1.length() - 1).c_str());
        remove(pathCur1.c_str());
    }
    return true;
}
void fileEncode()
{
    printf("开始加密...\n");
    char   buffer[MAX_PATH];
    getcwd(buffer, MAX_PATH);
    doFile(buffer, encodeFilter);
    printf("加密完成...\n\n\n");
}
void fileDecode()
{
    printf("开始解密...\n");
    char   buffer[MAX_PATH];
    getcwd(buffer, MAX_PATH);
    doFile(buffer, decodeFilter);
    printf("解密完成...\n\n\n");
}
//*******************************************************
void main()
{
AAA:
    printf("*********************************************************\n");
    printf("********************  加密解密  *************************\n");
    printf("*********************************************************\n");
    printf("●请输入密码\n");
    char buff1[64] = { 0 };
    gets_s(buff1);
    if (strlen(buff1) == 0)
        goto AAA;
    MD5Buff16(buff1);
BBB:
    printf("●选择需要的操作\n");
    printf("【1】加密\n");
    printf("【2】解密\n");
    char buff2[16] = { 0 };
    gets_s(buff2);
    if (strlen(buff2) == 0)
        goto BBB;
    if (atoi(buff2) == 1)
        fileEncode();
    else if (atoi(buff2) == 2)
        fileDecode();
    char buff3[16] = { 0 };
    gets_s(buff3);
    if (strlen(buff3) != 0)
        goto AAA;
}