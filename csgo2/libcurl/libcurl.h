#pragma once

#include <string>

#define CURL_STATICLIB
#include "inc/curl/curl.h"
#pragma comment(lib,"crypt32.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"wldap32.lib")

#ifndef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"_x32/_Debug/libeay32.lib")
#pragma comment(lib,"_x32/_Debug/ssleay32.lib")
#pragma comment(lib,"_x32/_Debug/libcurld.lib")
#else
#pragma comment(lib,"_x32/_Release/libeay32.lib")
#pragma comment(lib,"_x32/_Release/ssleay32.lib")
#pragma comment(lib,"_x32/_Release/libcurl.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib,"_x64/_Debug/libeay32.lib")
#pragma comment(lib,"_x64/_Debug/ssleay32.lib")
#pragma comment(lib,"_x64/_Debug/libcurld.lib")
#else
#pragma comment(lib,"_x64/_Release/libeay32.lib")
#pragma comment(lib,"_x64/_Release/ssleay32.lib")
#pragma comment(lib,"_x64/_Release/libcurl.lib")
#endif
#endif

extern "C" unsigned char* MD5(const unsigned char* d, size_t n, unsigned char* md);
extern "C" unsigned char* SHA1(const unsigned char* d, size_t n, unsigned char* md);

typedef CURL* curl_handle;
typedef CURLcode curl_code;

bool curl_get(std::string url, std::string* pbuffer, curl_slist* headers, bool tls, bool proxy);
bool curl_post(std::string url, std::string* pbuffer, std::string data, curl_slist* headers, bool tls, bool proxy);
void curl_init();
void curl_cleanup();