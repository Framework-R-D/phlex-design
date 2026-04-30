CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(graph_const_test VERSION 0.1 LANGUAGES CXX)
SET(CMAKE_CXX_STANDARD 23)
FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(fmt REQUIRED)

ADD_LIBRARY(graph-calc identifier.cpp calculate.cpp layer_path_t.cpp definitions.cpp Graph.cpp)
TARGET_LINK_LIBRARIES(graph-calc PUBLIC Boost::boost fmt::fmt)

ADD_EXECUTABLE(test test.cpp)
TARGET_LINK_LIBRARIES(test PRIVATE graph-calc)

ADD_EXECUTABLE(chat-gpt-1 chat_gpt_1.cpp)
TARGET_LINK_LIBRARIES(chat-gpt-1 PRIVATE graph-calc)

ADD_EXECUTABLE(stress-test stress_test.cpp)
TARGET_LINK_LIBRARIES(stress-test PRIVATE graph-calc)
