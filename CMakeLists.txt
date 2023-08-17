cmake_minimum_required(VERSION 3.20)
project(StrawberryCore)
set(CMAKE_CXX_STANDARD 23)


find_package(OpenSSL REQUIRED)
find_library(FMT_LIBRARY NAMES libfmt.a)
find_path(FMT_INCLUDE_DIR REQUIRED
		NAMES fmt/format.h
		HINTS /usr/local/include)
find_path(JSON_INCLUDE_DIR NAMES nlohmann/json.hpp)


add_library(StrawberryCore STATIC
		include/Strawberry/Core/Util/Assert.hpp
		include/Strawberry/Core/Util/Clock.hpp
		include/Strawberry/Core/Collection/CircularBuffer.hpp
		include/Strawberry/Core/Util/Endian.hpp
		include/Strawberry/Core/IO/Base64.hpp
		include/Strawberry/Core/IO/ByteBuffer.hpp
		include/Strawberry/Core/IO/Concepts.hpp
		include/Strawberry/Core/IO/DynamicByteBuffer.hpp
		include/Strawberry/Core/IO/Error.hpp
		include/Strawberry/Core/Util/Markers.hpp
		include/Strawberry/Core/Math/Math.hpp
		include/Strawberry/Core/Math/Math.inl
		include/Strawberry/Core/Math/Periodic.hpp
		include/Strawberry/Core/Math/Rational.hpp
		include/Strawberry/Core/Sync/Mutex.hpp
		include/Strawberry/Core/Net/Address.hpp
		include/Strawberry/Core/Net/Endpoint.hpp
		include/Strawberry/Core/Net/Error.hpp
		include/Strawberry/Core/Net/HTTP/Client.hpp
		include/Strawberry/Core/Net/HTTP/Client.inl
		include/Strawberry/Core/Net/HTTP/Constants.hpp
		include/Strawberry/Core/Net/HTTP/Header.hpp
		include/Strawberry/Core/Net/HTTP/Request.hpp
		include/Strawberry/Core/Net/HTTP/Response.hpp
		include/Strawberry/Core/Net/Socket/Socket.inl
		include/Strawberry/Core/Net/Socket/SocketAPI.hpp
		include/Strawberry/Core/Net/Socket/SocketAPIUser.hpp
		include/Strawberry/Core/Net/Socket/TCPClient.hpp
		include/Strawberry/Core/Net/Socket/TLSClient.hpp
		include/Strawberry/Core/Net/Socket/UDPClient.hpp
		include/Strawberry/Core/Net/Websocket/Client.hpp
		include/Strawberry/Core/Net/Websocket/Client.inl
		include/Strawberry/Core/Net/Websocket/Message.hpp
		include/Strawberry/Core/Util/Option.hpp
		include/Strawberry/Core/Util/Result.hpp
		include/Strawberry/Core/Util/ScopedTimer.hpp
		include/Strawberry/Core/Util/Utilities.hpp
		src/Util/Clock.cpp
		src/IO/Base64.cpp
		src/IO/DynamicByteBuffer.cpp
		src/Util/Logging.cpp
		src/Util/Markers.cpp
		src/Net/Address.cpp
		src/Net/Endpoint.cpp
		src/Net/HTTP/Client.cpp
		src/Net/HTTP/Constants.cpp
		src/Net/HTTP/Header.cpp
		src/Net/HTTP/Request.cpp
		src/Net/HTTP/Response.cpp
		src/Net/Socket/SocketAPI.cpp
		src/Net/Socket/SocketAPIUser.cpp
		src/Net/Socket/TCPClient.cpp
		src/Net/Socket/TLSClient.cpp
		src/Net/Socket/UDPClient.cpp
		src/Net/Websocket/Client.cpp
		src/Net/Websocket/Message.cpp
		src/Util/ScopedTimer.cpp
		include/Strawberry/Core/Thread/RepeatingTask.hpp
		src/Util/Metronome.cpp
		include/Strawberry/Core/Util/Metronome.hpp
		include/Strawberry/Core/IO/ChannelBroadcaster.hpp
		include/Strawberry/Core/IO/ChannelReceiver.hpp
		include/Strawberry/Core/Util/Variant.hpp
		include/Strawberry/Core/Sync/ConditionVariable.cpp
		include/Strawberry/Core/Sync/ConditionVariable.hpp
		include/Strawberry/Core/Util/Flag.cpp
		include/Strawberry/Core/Util/Flag.hpp
)

target_include_directories(StrawberryCore PUBLIC include ${OPENSSL_INCLUDE_DIR} ${FMT_INCLUDE_DIR} ${JSON_INCLUDE_DIR})
target_link_libraries(StrawberryCore PRIVATE OpenSSL::SSL OpenSSL::Crypto ${FMT_LIBRARY})

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	target_link_libraries(StrawberryCore PRIVATE ws2_32)
endif ()


add_executable(StrawberryCoreTest
		test/Main.cpp)
target_link_libraries(StrawberryCoreTest PRIVATE StrawberryCore)