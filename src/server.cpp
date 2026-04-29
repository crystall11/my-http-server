#include "http_parser.h"
#include "http_response.h"
#include <asm-generic/socket.h>
#include <cerrno>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

#define MAX_EVENTS 64

struct Connection {
  int fd;
  std::string read_buf;
  HttpParser parser;
};

std::unordered_map<int, Connection> connections;

// 把 fd 设为非阻塞
void set_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
void run_server(int port) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0); // 创建 socket
  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  // 在断开后可以立刻复用端口
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);
  int n = bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
  // 绑定端口
  listen(server_fd, 5); // 最多监听5个客户端
  set_nonblocking(server_fd);

  // --- 新增：创建 epoll 实例，把 server_fd 注册进去 ---
  int epfd = epoll_create1(0);

  struct epoll_event ev;
  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = server_fd;
  epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev);

  struct epoll_event events[MAX_EVENTS];

  while (1) {
    int n = epoll_wait(epfd, events, MAX_EVENTS, -1);

    for (int i = 0; i < n; i++) {
      int fd = events[i].data.fd;

      if (fd == server_fd) { // 建立客户端和服务器的连接
        while (1) {
          int client_fd = accept(server_fd, NULL, NULL);
          if (client_fd == -1) {
            if (errno == EAGAIN)
              break; // 没有新连接了
            else
              break; // 真正的错误
          }
          set_nonblocking(client_fd);

          connections[client_fd] = Connection{client_fd};

          ev.events = EPOLLIN | EPOLLET;
          ev.data.fd = client_fd;
          epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
          printf("new client connected, fd=%d\n", client_fd);
        }
      } else {
        Connection &conn = connections[fd];
        //读数据到conn.read_buf
        while (1) {
          char buf[1024];
          int bytes = read(fd, buf, sizeof(buf));
          if (bytes == -1 && errno == EAGAIN) {
            break;
          }
          if (bytes <= 0) {
            // 客户端断开
            printf("client fd=%d disconnected.\n", fd);
            epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
            close(fd);
            break;
          }
          conn.read_buf.append(buf,bytes);
        }
          
        // 判断是否接收完毕
        if (conn.read_buf.find("\r\n\r\n") == std::string::npos)
          continue;
        //解析请求
        bool done = conn.parser.parse(conn.read_buf);
        if (done) {
          auto &req = conn.parser.request;
          printf("method: %s\n", req.method.c_str());
          printf("path: %s\n", req.path.c_str());

          // 响应
          std::string response = serve_file(req.path);
          write(fd, response.c_str(), response.size());
          epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
          close(fd);
          connections.erase(fd);
        }
      }
    }
  }
}