cmake_minimum_required(VERSION 3.20)
project(StrawberryCore)

if (NOT TARGET StrawberryCore)
	include(FetchContent)
	include(ExternalProject)


	FetchContent_Declare(FMT
			EXCLUDE_FROM_ALL
			GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
			GIT_TAG "10.1.1")
	FetchContent_MakeAvailable(FMT)

	FetchContent_Declare(JSON
			EXCLUDE_FROM_ALL
			GIT_REPOSITORY "https://github.com/nlohmann/json.git"
			GIT_TAG "v3.11.3")
	FetchContent_MakeAvailable(JSON)

	FetchContent_Declare(STB
			EXCLUDE_FROM_ALL
			GIT_REPOSITORY "https://github.com/nothings/stb.git"
			CONFIGURE_COMMAND ""
			BUILD_COMMAND ""
			INSTALL_COMMAND ""
	)
	FetchContent_MakeAvailable(STB)
	FetchContent_GetProperties(STB
			SOURCE_DIR STB_SOURCE_DIR)


	add_library(StrawberryCore STATIC
			src/Strawberry/Core/Assert.hpp
			src/Strawberry/Core/Collection/CircularBuffer.hpp
			src/Strawberry/Core/Concepts.hpp
			src/Strawberry/Core/IO/Base64.cpp
			src/Strawberry/Core/IO/Base64.hpp
			src/Strawberry/Core/IO/Broadcaster.hpp
			src/Strawberry/Core/IO/ByteBuffer.hpp
			src/Strawberry/Core/IO/CallbackChannelReceiver.hpp
			src/Strawberry/Core/IO/CallbackReceiver.hpp
			src/Strawberry/Core/IO/ChannelBroadcaster.hpp
			src/Strawberry/Core/IO/ChannelReceiver.hpp
			src/Strawberry/Core/IO/Concepts.hpp
			src/Strawberry/Core/IO/DynamicByteBuffer.cpp
			src/Strawberry/Core/IO/DynamicByteBuffer.hpp
			src/Strawberry/Core/IO/Endian.hpp
			src/Strawberry/Core/IO/Error.hpp
			src/Strawberry/Core/IO/Logging.cpp
			src/Strawberry/Core/IO/Receiver.hpp
			src/Strawberry/Core/IO/Table.cpp
			src/Strawberry/Core/IO/Table.hpp
			src/Strawberry/Core/Markers.cpp
			src/Strawberry/Core/Markers.hpp
			src/Strawberry/Core/Math/AABB.hpp
			src/Strawberry/Core/Math/Clamped.hpp
			src/Strawberry/Core/Math/Math.hpp
			src/Strawberry/Core/Math/Math.inl
			src/Strawberry/Core/Math/Matrix.hpp
			src/Strawberry/Core/Math/Periodic.hpp
			src/Strawberry/Core/Math/Rational.hpp
			src/Strawberry/Core/Math/Transformations.hpp
			src/Strawberry/Core/Math/Units.hpp
			src/Strawberry/Core/Math/Vector.hpp
			src/Strawberry/Core/Sync/ConditionVariable.cpp
			src/Strawberry/Core/Sync/ConditionVariable.hpp
			src/Strawberry/Core/Sync/Mutex.hpp
			src/Strawberry/Core/Thread/RepeatingTask.hpp
			src/Strawberry/Core/Timing/Clock.cpp
			src/Strawberry/Core/Timing/Clock.hpp
			src/Strawberry/Core/Timing/Metronome.cpp
			src/Strawberry/Core/Timing/Metronome.hpp
			src/Strawberry/Core/Timing/ScopedTimer.cpp
			src/Strawberry/Core/Timing/ScopedTimer.hpp
			src/Strawberry/Core/Types/CopyOnWrite.hpp
			src/Strawberry/Core/Types/Date.cpp
			src/Strawberry/Core/Types/DynamicValue.hpp
			src/Strawberry/Core/Types/Lazy.hpp
			src/Strawberry/Core/Types/NullType.hpp
			src/Strawberry/Core/Types/Optional.hpp
			src/Strawberry/Core/Types/ReflexivePointer.hpp
			src/Strawberry/Core/Types/Result.hpp
			src/Strawberry/Core/Types/Uninitialised.hpp
			src/Strawberry/Core/Types/Variant.hpp
			src/Strawberry/Core/UTF.cpp
			src/Strawberry/Core/UTF.hpp
			src/Strawberry/Core/Util/IDPool.hpp
			src/Strawberry/Core/Util/Strings.hpp
            src/Strawberry/Core/Types/Date.hpp
            src/Strawberry/Core/Types/Delayed.hpp
	)

	target_include_directories(StrawberryCore
			PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
			PRIVATE ${STB_SOURCE_DIR})
	target_link_libraries(StrawberryCore PUBLIC nlohmann_json::nlohmann_json fmt::fmt-header-only)
	set_target_properties(StrawberryCore PROPERTIES CXX_STANDARD 20)


	add_executable(StrawberryCoreTest
			test/Main.cpp)
	target_link_libraries(StrawberryCoreTest PRIVATE StrawberryCore)
	set_target_properties(StrawberryCoreTest PROPERTIES CXX_STANDARD 20)
endif()
