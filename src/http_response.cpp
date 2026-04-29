#include "http_response.h"
#include <fstream>
#include <string>

// 根据路径猜 Content-Type
std::string guess_content_type(const std::string& path){
    if(path.find(".html")!=std::string::npos)return "text/html";
    if(path.find(".json")!=std::string::npos)return "application/json";
    if(path.find(".css")!=std::string::npos)return "text/css";
    if(path.find(".png")!=std::string::npos)return "img/png";
    if(path.find(".out")!=std::string::npos)return "application/octet-stream";
    return "";
}

// 构造完整 HTTP 响应
std::string make_response(int status, const std::string& body,const std::string& content_type){
    std::string status_text = (status == 200) ? "OK" : "Not Found";
    std::string response = "HTTP/1.1 " + std::to_string(status) + " " + status_text + "\r\n"
                           "Content-Type: " + content_type + "\r\n"
                           "Content-Length: " + std::to_string(body.size()) + "\r\n"
                           "Connection: close\r\n"
                           "\r\n"
                         + body; 
    return response;
}

// 读文件，返回完整响应字符串
std::string serve_file(const std::string& path){
    std::string realpath="./static"+path;
    printf("serving file: %s\n", realpath.c_str());  // ← 加这行
    std::ifstream file(realpath);

    if(!file.is_open()){
        printf("file not found\n");  // ← 加这行
        return make_response(404, "Not Found", "text/plain");
    }
    else {
        std::string body((std::istreambuf_iterator<char>(file)),
                  std::istreambuf_iterator<char>());
        printf("file size: %zu bytes\n", body.size());  // ← 加这行
        std::string response = make_response(200, body, guess_content_type(path));
        printf("response size: %zu bytes\n", response.size());  // ← 加这行
        return response;
    }
}