#include <iostream>

int main(int argc, char* argv[]) {
    int port = 8080;
    if (argc > 1) port = std::stoi(argv[1]);
    
    std::cout << "[server] starting on port " << port << std::endl;
    // TODO: initialize epoll server
    return 0;
}