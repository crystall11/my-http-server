// http_response.h
#pragma once
#include <string>

// 根据路径猜 Content-Type
std::string guess_content_type(const std::string &path);

// 构造完整 HTTP 响应
std::string make_response(int status, const std::string &body,
                          const std::string &content_type, bool keep_alive);

// 读文件，返回完整响应字符串
std::string serve_file(const std::string &path, bool keep_alive);
/*
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/plain\r\n"
"Content-Length: 13\r\n"
"Connection: close\r\n"
 "\r\n"
 "Hello, World!";
*/