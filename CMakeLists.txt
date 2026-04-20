cmake_minimum_required(VERSION 3.16)
project(my-http-server CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # 给 clangd 用

# ASan（Debug 模式）
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address)
endif()

# 收集源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(server ${SOURCES})
target_include_directories(server PRIVATE include src)
target_link_libraries(server PRIVATE pthread)