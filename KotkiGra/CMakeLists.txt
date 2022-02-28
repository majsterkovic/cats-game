cmake_minimum_required(VERSION 3.20)
project(KotkiGra)

set(CMAKE_CXX_STANDARD 23)


set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "C:/SFML/lib")

find_package(SFML 2.5 COMPONENTS graphics audio system window REQUIRED)


add_executable(KotkiGra main.cpp)
target_link_libraries(KotkiGra sfml-graphics sfml-audio sfml-system sfml-window)