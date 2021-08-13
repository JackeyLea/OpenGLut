# CMake最低版本号
cmake_minimum_required(VERSION 3.5)

#项目信息
project(tga_load)

#C++标准
set(CMAKE_CXX_STANDARD 11)

#生成可执行文件
add_executable(tga_load tga_load.cpp)

#链接库位置
link_directories(/usr/lib)

#链接GL库
target_link_libraries(tga_load GL glut GLU GLX X11 dl rt -pthread -static-libgcc stdc++)
