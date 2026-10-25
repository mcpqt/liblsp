cmake_minimum_required(VERSION 3.20)

project(liblsp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)

find_package(Threads REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Network)

set(BUILD_TESTING OFF)
add_subdirectory(libqjsonrpc EXCLUDE_FROM_ALL)

add_library(lsp
    src/LanguageClient.h
    src/LanguageServer.h
    src/Logger.h
    src/lsp.h
    src/LSPLogger.h
    src/protocol/LSProtocol.h
    src/Remote.h
    src/StdIoLogger.h
    src/protocol/LSProtocol.cpp
    src/lsp.cpp
)

target_include_directories(lsp PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(lsp PUBLIC qjsonrpc Qt6::Core Qt6::Network)
set_target_properties(lsp PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

add_executable(Simple
    tests/Simple.cpp
)

target_link_libraries(Simple PRIVATE lsp)
set_target_properties(Simple PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
