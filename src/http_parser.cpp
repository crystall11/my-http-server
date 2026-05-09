#include "http_parser.h"
#include <cstddef>
#include <sstream>

bool HttpParser::parse(const std::string &raw) {
  size_t pos = 0;
  while (pos < raw.size()) {
    size_t end = raw.find("\r\n", pos);
    if (end == std::string::npos)
      break;

    std::string line = raw.substr(pos, end - pos);
    pos = end + 2; // skip"\r\n"

    if (state == ParseState::REQUEST_LINE) {
      std::istringstream ss(line);
      ss >> request.method >> request.path >> request.version;
      state = ParseState::HEADERS;
    } else if (state == ParseState::HEADERS) {
      if (line.empty()) {
        // empty line.requestheader finish
        state = ParseState::DONE;
        return true;
      }
      // parse headers
      size_t colon = line.find(":");
      std::string key = line.substr(0, colon);
      std::string value = line.substr(colon + 2);
      request.headers[key] = value;
    }
  }
  return state == ParseState::DONE;
}