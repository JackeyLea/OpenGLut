# CMake最低版本号
cmake_minimum_required(VERSION 3.5)

#项目信息
project(lighting)

#C++标准
set(CMAKE_CXX_STANDARD 11)

#生成可执行文件
add_executable(lighting lighting.cpp)

#链接库位置
link_directories(/usr/lib)

#链接GL库
target_link_libraries(lighting GL glut GLU)