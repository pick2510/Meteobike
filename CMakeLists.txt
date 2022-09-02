cmake_minimum_required(VERSION 3.10)

# set the project name
project(meteobike)

include_directories(include)
# add the executable
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(meteobike  src/meteobike.cpp)
target_sources(meteobike PRIVATE src/writer.cpp)
