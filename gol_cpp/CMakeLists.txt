cmake_minimum_required(VERSION 3.17)
project(GOL_CPP)

set(CMAKE_CXX_STANDARD 20)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
add_executable(GOL_CPP main.cpp)
target_link_libraries(GOL_CPP sfml-graphics sfml-audio)

## References:
# https://en.sfml-dev.org/forums/index.php?topic=24070.0
# https://github.com/SFML/SFML/blob/master/cmake/SFMLConfig.cmake.in
