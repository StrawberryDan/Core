cmake_minimum_required(VERSION 3.0)
project(StrawberryCore)
set(CMAKE_CXX_STANDARD 23)



find_package(OpenSSL REQUIRED)
find_library(FMT_LIBRARY NAMES libfmt.a)
find_path(JSON_INCLUDE_DIR NAMES nlohmann/json.hpp)



add_library(StrawberryCore STATIC
		include/Strawberry/Core/Assert.hpp
		include/Strawberry/Core/Clock.hpp
		include/Strawberry/Core/Collection/CircularBuffer.hpp
		include/Strawberry/Core/Concepts.hpp
		include/Strawberry/Core/Endian.hpp
		include/Strawberry/Core/IO/Base64.hpp
		include/Strawberry/Core/IO/ByteBuffer.hpp
        include/Strawberry/Core/IO/Concepts.hpp
		include/Strawberry/Core/IO/DynamicByteBuffer.hpp
		include/Strawberry/Core/IO/Error.hpp
		include/Strawberry/Core/Markers.hpp
		include/Strawberry/Core/Math/Math.hpp
		include/Strawberry/Core/Mutex.hpp
		include/Strawberry/Core/Net/Address.hpp
		include/Strawberry/Core/Net/Endpoint.hpp
		include/Strawberry/Core/Net/Error.hpp
		include/Strawberry/Core/Net/HTTP/Client.hpp
		include/Strawberry/Core/Net/HTTP/Constants.hpp
		include/Strawberry/Core/Net/HTTP/Header.hpp
		include/Strawberry/Core/Net/HTTP/Request.hpp
		include/Strawberry/Core/Net/HTTP/Response.hpp
		include/Strawberry/Core/Net/Socket/SocketAPI.hpp
		include/Strawberry/Core/Net/Socket/TCPClient.hpp
		include/Strawberry/Core/Net/Socket/TLSClient.hpp
		include/Strawberry/Core/Net/Socket/UDPClient.hpp
		include/Strawberry/Core/Net/Websocket/Client.hpp
		include/Strawberry/Core/Net/Websocket/Message.hpp
		include/Strawberry/Core/Option.hpp
		include/Strawberry/Core/Result.hpp
		include/Strawberry/Core/Utilities.hpp
		include/Strawberry/Core/Collection/CircularBuffer.inl
		include/Strawberry/Core/Math/Math.inl
		include/Strawberry/Core/Net/HTTP/Client.inl
		include/Strawberry/Core/Net/Socket/Socket.inl
		include/Strawberry/Core/Net/Websocket/Client.inl
		src/Clock.cpp
		src/IO/Base64.cpp
        src/IO/DynamicByteBuffer.cpp
        src/Logging.cpp
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
		include/Strawberry/Core/Net/Socket/SocketAPIUser.hpp
		src/Net/Socket/SocketAPIUser.cpp
		include/Strawberry/Core/Math/Periodic.hpp
		include/Strawberry/Core/IO/Circular.hpp)

target_include_directories(StrawberryCore PUBLIC include inline ${OPENSSL_INCLUDE_DIR} ${JSON_INCLUDE_DIR})
target_link_libraries(StrawberryCore PRIVATE OpenSSL::SSL OpenSSL::Crypto ${FMT_LIBRARY})

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	target_link_libraries(StrawberryCore PRIVATE ws2_32)
endif()



add_executable(StrawberryCoreTest
		test/Main.cpp)
target_link_libraries(StrawberryCoreTest PRIVATE StrawberryCore)