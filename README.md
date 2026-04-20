# my-http-server

轻量级 HTTP/1.1 服务器，基于 epoll ET 模式 + 线程池，支持 AI API 转发。

## 特性
- [ ] epoll ET 模式非阻塞 IO
- [ ] 线程池处理并发请求
- [ ] HTTP/1.1 GET / POST 解析
- [ ] 静态文件服务
- [ ] Keep-Alive 长连接
- [ ] AI API 反向代理

## 构建

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/server 8080
```

## 设计说明

（后续补充：架构图、模块说明）