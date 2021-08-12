# CMake最低版本号
cmake_minimum_required(VERSION 3.5)

#项目信息
project(polygon_colored)

#C++标准
set(CMAKE_CXX_STANDARD 11)

#生成可执行文件
add_executable(polygon_colored polygon_colored.cpp)

#链接库位置
link_directories(/usr/lib)
#链接GL库
target_link_libraries(polygon_colored GL glut GLU)