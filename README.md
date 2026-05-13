# my-http-server

轻量级 HTTP/1.1 服务器，基于 epoll ET 模式，用 C++ 从零实现。

## 已实现功能

✅ epoll ET 非阻塞 IO
✅ HTTP/1.1 GET 解析（状态机）
✅ 静态文件服务
✅ 线程池
✅ Keep-Alive
✅ 超时断开
⬜ 压测 + README 数据
⬜ POST 请求

## 构建 & 运行

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/server 8080
```

## 测试

```bash
# 正常请求
curl -i http://127.0.0.1:8080/index.html

# 404
curl -i http://127.0.0.1:8080/notexist
```

## 性能测试

环境：WSL2 Ubuntu 22.04，4 核，wrk 压测静态文件

| 并发连接数 | QPS | 平均延迟 |
|-----------|-----|---------|
| 1 | 3,757 | 0.26ms |
| 100 | 7,600 | 13.28ms |
| 500 | 7,544 | 46.29ms |

测试命令：`wrk -t4 -c100 -d30s http://127.0.0.1:8080/index.html`

## 技术设计

- 网络层：epoll ET + 非阻塞 fd，每个连接维护独立读缓冲区处理粘包
- 解析层：状态机逐行解析 HTTP 请求头（REQUEST_LINE → HEADERS → DONE）
- 响应层：根据文件扩展名自动设置 Content-Type