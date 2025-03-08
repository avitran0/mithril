cmake_minimum_required(VERSION 3.28)
project(mithril CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

add_library(mithril ${SOURCE_FILES})

target_include_directories(mithril PUBLIC include)
