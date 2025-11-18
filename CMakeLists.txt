cmake_minimum_required(VERSION 3.14)
project(CompletadorDePalavras LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(completador
    main.cpp
)

target_include_directories(completador PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(completador PRIVATE sfml-graphics sfml-window sfml-system)
