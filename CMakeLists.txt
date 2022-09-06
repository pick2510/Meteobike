cmake_minimum_required(VERSION 3.10)

# set the project name
project(meteobike)

include_directories(include)
# add the executable
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file( GLOB SRCS src/*.c src/*.cpp )

add_executable(meteobike  ${SRCS})
target_link_libraries(meteobike gps pthread)
#target_sources(meteobike PRIVATE ${SRCS})
