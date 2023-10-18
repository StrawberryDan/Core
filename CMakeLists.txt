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
		src/Strawberry/Core/Assert.hpp
		src/Strawberry/Core/Timing/Clock.hpp
		src/Strawberry/Core/Collection/CircularBuffer.hpp
		src/Strawberry/Core/IO/Endian.hpp
		src/Strawberry/Core/IO/Base64.hpp
		src/Strawberry/Core/IO/ByteBuffer.hpp
		src/Strawberry/Core/IO/Concepts.hpp
		src/Strawberry/Core/IO/DynamicByteBuffer.hpp
		src/Strawberry/Core/IO/Error.hpp
		src/Strawberry/Core/Markers.hpp
		src/Strawberry/Core/Math/Math.hpp
		src/Strawberry/Core/Math/Math.inl
		src/Strawberry/Core/Math/Periodic.hpp
		src/Strawberry/Core/Math/Rational.hpp
		src/Strawberry/Core/Sync/Mutex.hpp
		src/Strawberry/Core/Net/Address.hpp
		src/Strawberry/Core/Net/Endpoint.hpp
		src/Strawberry/Core/Net/Error.hpp
		src/Strawberry/Core/Net/HTTP/Client.hpp
		src/Strawberry/Core/Net/HTTP/Client.inl
		src/Strawberry/Core/Net/HTTP/Constants.hpp
		src/Strawberry/Core/Net/HTTP/Header.hpp
		src/Strawberry/Core/Net/HTTP/Request.hpp
		src/Strawberry/Core/Net/HTTP/Response.hpp
		src/Strawberry/Core/Net/Socket/API.hpp
		src/Strawberry/Core/Net/Socket/TCPClient.hpp
		src/Strawberry/Core/Net/Socket/TLSClient.hpp
		src/Strawberry/Core/Net/Socket/UDPClient.hpp
		src/Strawberry/Core/Net/Websocket/Client.hpp
		src/Strawberry/Core/Net/Websocket/Client.inl
		src/Strawberry/Core/Net/Websocket/Message.hpp
		src/Strawberry/Core/Types/Optional.hpp
		src/Strawberry/Core/Types/Result.hpp
		src/Strawberry/Core/Timing/ScopedTimer.hpp
		src/Strawberry/Core/Util/Stubs.hpp
		src/Strawberry/Core/Timing/Clock.cpp
		src/Strawberry/Core/IO/Base64.cpp
		src/Strawberry/Core/IO/DynamicByteBuffer.cpp
		src/Strawberry/Core/IO/Logging.cpp
		src/Strawberry/Core/Markers.cpp
		src/Strawberry/Core/Net/Address.cpp
		src/Strawberry/Core/Net/Endpoint.cpp
		src/Strawberry/Core/Net/HTTP/Client.cpp
		src/Strawberry/Core/Net/HTTP/Constants.cpp
		src/Strawberry/Core/Net/HTTP/Header.cpp
		src/Strawberry/Core/Net/HTTP/Request.cpp
		src/Strawberry/Core/Net/HTTP/Response.cpp
		src/Strawberry/Core/Net/Socket/API.cpp
		src/Strawberry/Core/Net/Socket/TCPClient.cpp
		src/Strawberry/Core/Net/Socket/TLSClient.cpp
		src/Strawberry/Core/Net/Socket/UDPClient.cpp
		src/Strawberry/Core/Net/Websocket/Client.cpp
		src/Strawberry/Core/Net/Websocket/Message.cpp
		src/Strawberry/Core/Timing/ScopedTimer.cpp
		src/Strawberry/Core/Thread/RepeatingTask.hpp
		src/Strawberry/Core/Timing/Metronome.cpp
		src/Strawberry/Core/Timing/Metronome.hpp
		src/Strawberry/Core/IO/ChannelBroadcaster.hpp
		src/Strawberry/Core/IO/ChannelReceiver.hpp
		src/Strawberry/Core/Types/Variant.hpp
		src/Strawberry/Core/Sync/ConditionVariable.cpp
		src/Strawberry/Core/Sync/ConditionVariable.hpp
		src/Strawberry/Core/Types/Lazy.hpp
		src/Strawberry/Core/Types/CopyOnWrite.hpp
		src/Strawberry/Core/Util/IDPool.hpp
		src/Strawberry/Core/Types/MaybeUninitialised.hpp
		src/Strawberry/Core/IO/Broadcaster.hpp
		src/Strawberry/Core/IO/Receiver.hpp
		src/Strawberry/Core/IO/CallbackReceiver.hpp
		src/Strawberry/Core/Types/ReflexivePointer.hpp
		src/Strawberry/Core/Math/Vector.hpp
		src/Strawberry/Core/Math/Units.hpp
		src/Strawberry/Core/Math/Units.cpp
		src/Strawberry/Core/Math/Matrix.hpp
		src/Strawberry/Core/Types/NullType.hpp
		src/Strawberry/Core/Concepts.hpp
		src/Strawberry/Core/UTF.hpp
		src/Strawberry/Core/UTF.cpp
)

target_include_directories(StrawberryCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(StrawberryCore PUBLIC OpenSSL::SSL OpenSSL::Crypto nlohmann_json::nlohmann_json fmt::fmt-header-only)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	target_link_libraries(StrawberryCore PUBLIC ws2_32)
endif ()


add_executable(StrawberryCoreTest
		test/Main.cpp)
target_link_libraries(StrawberryCoreTest PRIVATE StrawberryCore)
