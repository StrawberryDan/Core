cmake_minimum_required(VERSION 3.0)
project(Standard)
set(CMAKE_CXX_STANDARD 20)
find_package(PkgConfig REQUIRED)

find_package(OpenSSL REQUIRED)

find_library(FMT_LIBRARY
        NAMES libfmt.a)

find_path(JSON_INCLUDE_DIR
        NAMES nlohmann/json.hpp)



add_library(Standard STATIC
        include/Standard/Assert.hpp
        include/Standard/Clock.hpp
        include/Standard/Collection/CircularBuffer.hpp
        include/Standard/Concepts.hpp
        include/Standard/Endian.hpp
        include/Standard/IO/Base64.hpp
        include/Standard/IO/BufferedIO.hpp
        include/Standard/IO/BufferedReader.hpp
        include/Standard/IO/BufferedWriter.hpp
        include/Standard/IO/ByteBuffer.hpp
        include/Standard/IO/Concepts.hpp
        include/Standard/IO/DynamicByteBuffer.hpp
        include/Standard/IO/Error.hpp
        include/Standard/Iter/Filter.hpp
        include/Standard/Iter/Iterator.hpp
        include/Standard/Iter/Map.hpp
        include/Standard/Iter/STLWrapper.hpp
        include/Standard/Iter/STLWrapper.hpp
        include/Standard/Iter/Skip.hpp
        include/Standard/Markers.hpp
        include/Standard/Math/Math.hpp
        include/Standard/Mutex.hpp
        include/Standard/Net/Address.hpp
        include/Standard/Net/Endpoint.hpp
        include/Standard/Net/Error.hpp
        include/Standard/Net/HTTP/Client.hpp
        include/Standard/Net/HTTP/Constants.hpp
        include/Standard/Net/HTTP/Header.hpp
        include/Standard/Net/HTTP/Request.hpp
        include/Standard/Net/HTTP/Response.hpp
        include/Standard/Net/Socket/TCPClient.hpp
        include/Standard/Net/Socket/TLSClient.hpp
        include/Standard/Net/Socket/UDPClient.hpp
        include/Standard/Net/Websocket/Client.hpp
        include/Standard/Net/Websocket/Message.hpp
        include/Standard/Option.hpp
        include/Standard/Result.hpp
        include/Standard/Utilities.hpp
        inline/Standard/Collection/CircularBuffer.inl
        inline/Standard/Math/Math.inl
        inline/Standard/Net/HTTP/Client.inl
        inline/Standard/Net/Socket/Socket.inl
        inline/Standard/Net/Websocket/Client.inl
        src/Clock.cpp
        src/IO/Base64.cpp
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
        src/Net/Socket/TCPClient.cpp
        src/Net/Socket/TLSClient.cpp
        src/Net/Socket/UDPClient.cpp
        src/Net/Websocket/Client.cpp
        src/Net/Websocket/Message.cpp
        include/Standard/IO/CircularByteBuffer.hpp include/Standard/IO/CircularDynamicByteBuffer.hpp src/IO/CircularDynamicByteBuffer.cpp)

target_include_directories(Standard PUBLIC include inline ${OPENSSL_INCLUDE_DIR} ${JSON_INCLUDE_DIR})
target_link_libraries(Standard PRIVATE OpenSSL::SSL OpenSSL::Crypto ${FMT_LIBRARY})



add_executable(Standard_Test
        test/Main.cpp)
target_link_libraries(Standard_Test PRIVATE Standard)