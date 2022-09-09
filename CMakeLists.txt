cmake_minimum_required(VERSION 3.10)

# set the project name
project(meteobike)

include_directories(include)
add_compile_definitions(USE_BCM2835_LIB)
add_compile_definitions(RPI)
#add_compile_definitions(DEBUG)
# add the executable
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(lib/e-Paper/RaspberryPi_JetsonNano/c/lib/Fonts lib/e-Paper/RaspberryPi_JetsonNano/c/lib/GUI lib/e-Paper/RaspberryPi_JetsonNano/c/lib/e-Paper lib/e-Paper/RaspberryPi_JetsonNano/c/lib/Config )
file( GLOB SRCS src/*.c src/*.cpp lib/e-Paper/RaspberryPi_JetsonNano/c/lib/Fonts/* lib/e-Paper/RaspberryPi_JetsonNano/c/lib/GUI/* lib/e-Paper/RaspberryPi_JetsonNano/c/lib/e-Paper/EPD_2in7* lib/e-Paper/RaspberryPi_JetsonNano/c/lib/Config/DEV*)

add_executable(meteobike  ${SRCS})
add_custom_command(TARGET meteobike POST_BUILD
                   COMMAND sudo setcap cap_sys_rawio+ep
                    ${CMAKE_CURRENT_BINARY_DIR}/meteobike)
target_link_libraries(meteobike gps pthread bcm2835 cap)
#target_sources(meteobike PRIVATE ${SRCS})
