CMAKE_MINIMUM_REQUIRED(VERSION 4.0)
PROJECT(graph_const_test VERSION 0.1 LANGUAGES CXX)
SET(CMAKE_CXX_STANDARD 23)
FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(fmt REQUIRED)

ADD_EXECUTABLE(graph-const main.cpp identifier.cpp calculate.cpp layer_path_t.cpp definitions.cpp Graph.cpp)
TARGET_LINK_LIBRARIES(graph-const PUBLIC Boost::boost fmt::fmt)

