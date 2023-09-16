cmake_minimum_required(VERSION 3.20)
project(StrawberryCore)
set(CMAKE_CXX_STANDARD 23)
include(FetchContent)


find_package(OpenSSL REQUIRED)

FetchContent_Declare(FMT
		GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
		GIT_TAG "10.1.1")
FetchContent_MakeAvailable(FMT)

FetchContent_Declare(JSON
		GIT_REPOSITORY "https://github.com/nlohmann/json.git"
		GIT_TAG "v3.11.2")
FetchContent_MakeAvailable(JSON)


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
		include/Strawberry/Core/Net/Socket/API.hpp
		include/Strawberry/Core/Net/Socket/TCPClient.hpp
		include/Strawberry/Core/Net/Socket/TLSClient.hpp
		include/Strawberry/Core/Net/Socket/UDPClient.hpp
		include/Strawberry/Core/Net/Websocket/Client.hpp
		include/Strawberry/Core/Net/Websocket/Client.inl
		include/Strawberry/Core/Net/Websocket/Message.hpp
		include/Strawberry/Core/Util/Optional.hpp
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
		src/Net/Socket/API.cpp
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
		include/Strawberry/Core/Util/Lazy.hpp
		include/Strawberry/Core/Util/Flag.cpp
		include/Strawberry/Core/Util/Flag.hpp
		include/Strawberry/Core/Util/COW.hpp
		include/Strawberry/Core/Util/IDGenerator.hpp
		include/Strawberry/Core/Util/MaybeUninitialised.hpp
)

target_include_directories(StrawberryCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(StrawberryCore PUBLIC OpenSSL::SSL OpenSSL::Crypto nlohmann_json::nlohmann_json fmt::fmt-header-only)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	target_link_libraries(StrawberryCore PUBLIC ws2_32)
endif ()


add_executable(StrawberryCoreTest
		test/Main.cpp)
target_link_libraries(StrawberryCoreTest PRIVATE StrawberryCore)