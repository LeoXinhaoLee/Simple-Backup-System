cmake_minimum_required(VERSION 3.20)
project(znfs_lib_4)

set(CMAKE_CXX_STANDARD 11)

add_library(znfs_lib_4 SHARED library.cpp simple_tar.cc tar/tar.cpp cp/copy_tool.cc cp/dir_node.cc cp/diff.cc)
