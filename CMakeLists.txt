cmake_minimum_required(VERSION 3.0)
project(Strawberry_Core)
set(CMAKE_CXX_STANDARD 20)



find_package(PkgConfig REQUIRED)
find_package(OpenSSL REQUIRED)
find_library(FMT_LIBRARY
        NAMES libfmt.a)
find_path(JSON_INCLUDE_DIR
        NAMES nlohmann/json.hpp)



add_library(Strawberry_Core STATIC
        include/Core/Assert.hpp
        include/Core/Clock.hpp
        include/Core/Collection/CircularBuffer.hpp
        include/Core/Concepts.hpp
        include/Core/Endian.hpp
        include/Core/IO/Base64.hpp
        include/Core/IO/ByteBuffer.hpp
        include/Core/IO/CircularByteBuffer.hpp
        include/Core/IO/CircularDynamicByteBuffer.hpp
        include/Core/IO/Concepts.hpp
        include/Core/IO/DynamicByteBuffer.hpp
        include/Core/IO/Error.hpp
        include/Core/Markers.hpp
        include/Core/Math/Math.hpp
        include/Core/Mutex.hpp
        include/Core/Net/Address.hpp
        include/Core/Net/Endpoint.hpp
        include/Core/Net/Error.hpp
        include/Core/Net/HTTP/Client.hpp
        include/Core/Net/HTTP/Constants.hpp
        include/Core/Net/HTTP/Header.hpp
        include/Core/Net/HTTP/Request.hpp
        include/Core/Net/HTTP/Response.hpp
        include/Core/Net/Socket/SocketAPI.hpp
        include/Core/Net/Socket/TCPClient.hpp
        include/Core/Net/Socket/TLSClient.hpp
        include/Core/Net/Socket/UDPClient.hpp
        include/Core/Net/Websocket/Client.hpp
        include/Core/Net/Websocket/Message.hpp
        include/Core/Option.hpp
        include/Core/Result.hpp
        include/Core/Utilities.hpp
        inline/Core/Collection/CircularBuffer.inl
        inline/Core/Math/Math.inl
        inline/Core/Net/HTTP/Client.inl
        inline/Core/Net/Socket/Socket.inl
        inline/Core/Net/Websocket/Client.inl
        src/Clock.cpp
        src/IO/Base64.cpp
        src/IO/CircularDynamicByteBuffer.cpp
        src/IO/DynamicByteBuffer.cpp
        src/Log.cpp
        src/Markers.cpp
        src/Net/Address.cpp
        src/Net/Endpoint.cpp
        src/Net/HTTP/Client.cpp
        src/Net/HTTP/Constants.cpp
        src/Net/HTTP/Header.cpp
        src/Net/HTTP/Request.cpp
        src/Net/HTTP/Response.cpp
        src/Net/RTP/Header.cpp
        src/Net/Socket/SocketAPI.cpp
        src/Net/Socket/TCPClient.cpp
        src/Net/Socket/TLSClient.cpp
        src/Net/Socket/UDPClient.cpp
        src/Net/Websocket/Client.cpp
        src/Net/Websocket/Message.cpp
        include/Core/Net/Socket/SocketAPIUser.hpp src/Net/Socket/SocketAPIUser.cpp)

target_include_directories(Strawberry_Core PUBLIC include inline ${OPENSSL_INCLUDE_DIR} ${JSON_INCLUDE_DIR})
target_link_libraries(Strawberry_Core PRIVATE OpenSSL::SSL OpenSSL::Crypto ${FMT_LIBRARY})

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(Strawberry_Core PRIVATE ws2_32)
endif()



add_executable(Strawberry_Core_Test
        test/Main.cpp)
target_link_libraries(Strawberry_Core_Test PRIVATE Strawberry_Core)