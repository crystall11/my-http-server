// src/main.cpp
#include <stdio.h>

// 声明 server.cpp 里的函数
void run_server(int port);

int main() {
    run_server(8080);
    return 0;
}