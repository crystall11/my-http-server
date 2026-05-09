#pragma once
#include <string>
#include <unordered_map>

enum class ParseState { REQUEST_LINE, HEADERS, DONE, ERROR };

struct HttpRequest {
  std::string method;
  std::string path;
  std::string version;
  std::unordered_map<std::string, std::string> headers;
};

struct HttpParser {
  ParseState state = ParseState::REQUEST_LINE;
  HttpRequest request;

  // 喂数据进来，返回是否解析完成
  bool parse(const std::string &raw);
};