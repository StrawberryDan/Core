cmake_minimum_required(VERSION 3.0)
project(Standard)
set(CMAKE_CXX_STANDARD 20)


find_library(FMT_LIBRARY
        NAMES libfmt.a)

find_library(SSL_LIBRARY,
        NAMES libssl.a)

find_library(TLS_LIBRARY
        NAMES libtls.a)

find_path(TLS_INCLUDE_DIR
        NAMES tls.h)

find_path(JSON_INCLUDE_DIR
        NAMES nlohmann/json.hpp)


add_library(LibreSSL INTERFACE)
target_link_libraries(LibreSSL INTERFACE ${SSL_LIBRARY} ${TLS_LIBRARY})
target_include_directories(LibreSSL INTERFACE ${TLS_INCLUDE_DIR})



add_library(Standard STATIC
        include/Standard/Base64.hpp
        include/Standard/Math/Math.hpp
        include/Standard/Math/Math.inl
        include/Standard/Net/HTTP/ChunkedPayload.hpp
        include/Standard/Net/HTTP/Client.hpp
        include/Standard/Net/HTTP/Client.inl
        include/Standard/Net/HTTP/Constants.hpp
        include/Standard/Net/HTTP/Header.hpp
        include/Standard/Net/HTTP/Payload.hpp
        include/Standard/Net/HTTP/Request.hpp
        include/Standard/Net/HTTP/Response.hpp
        include/Standard/Net/HTTP/SimplePayload.hpp
        include/Standard/Net/Socket/Socket.hpp
        include/Standard/Net/Socket/Socket.inl
        include/Standard/Net/Socket/TCPClient.hpp
        include/Standard/Net/Socket/TLSClient.hpp
        include/Standard/Net/Socket/UDPClient.hpp
        include/Standard/Net/Websocket/Client.hpp
        include/Standard/Utilities.hpp
        src/Base64.cpp
        src/Net/HTTP/ChunkedPayload.cpp
        src/Net/HTTP/Constants.cpp
        src/Net/HTTP/Header.cpp
        src/Net/HTTP/Request.cpp
        src/Net/HTTP/Response.cpp
        src/Net/HTTP/SimplePayload.cpp
        src/Net/Socket/TCPClient.cpp
        src/Net/Socket/TLSClient.cpp
        src/Net/Socket/UDPClient.cpp
        include/Standard/Net/Websocket/Client.inl
        include/Standard/Mutex.hpp
        src/Net/Websocket/Message.cpp
        include/Standard/Net/Websocket/Message.hpp
        include/Standard/Result.hpp
        src/Clock.cpp
        include/Standard/Clock.hpp
        include/Standard/Endian.hpp
        include/Standard/Option.hpp
        include/Standard/Assert.hpp
        include/Standard/Concepts.hpp
        include/Standard/Markers.hpp
        src/Markers.cpp
        src/Net/Websocket/Client.cpp
        src/Net/HTTP/Client.cpp
        include/Standard/Net/Address.hpp
        src/Net/Address.cpp
        include/Standard/Log.hpp
        src/Log.cpp)

target_include_directories(Standard PUBLIC include PRIVATE  ${JSON_INCLUDE_DIR})
target_link_libraries(Standard PRIVATE LibreSSL ${FMT_LIBRARY})



add_executable(Standard_Test
        test/Main.cpp)
target_link_libraries(Standard_Test PRIVATE Standard)