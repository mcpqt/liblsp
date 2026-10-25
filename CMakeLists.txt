cmake_minimum_required(VERSION 3.20)

project(liblsp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)

find_package(Threads REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_library(lsp
    src/rpc/error.h
    src/rpc/json.h
    src/rpc/jsonrpc.h
    src/rpc/log.h
    src/rpc/session.h
    src/rpc/stdio_transport.h
    src/rpc/transport.h
    src/rpc/qt_tcp_transport.h
    src/LanguageClient.h
    src/LanguageServer.h
    src/Logger.h
    src/lsp.h
    src/LSPLogger.h
    src/protocol/LSProtocol.h
    src/Remote.h
    src/StdIoLogger.h
    src/rpc/jsonrpc.cpp
    src/rpc/log.cpp
    src/rpc/session.cpp
    src/rpc/stdio_transport.cpp
    src/rpc/qt_tcp_transport.cpp
    src/protocol/LSProtocol.cpp
    src/lsp.cpp
)

target_include_directories(lsp PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(lsp PUBLIC Qt6::Core Qt6::Network)
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
