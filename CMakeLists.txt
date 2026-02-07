cmake_minimum_required(VERSION 3.20)
project(StrawberryCore)
enable_testing()
include(FetchContent)
include(ExternalProject)


if (PROJECT_IS_TOP_LEVEL)
    FetchContent_Declare(StrawberryConfig
            GIT_REPOSITORY "https://github.com/StrawberryDan/Config.git"
            GIT_TAG "main")
    FetchContent_MakeAvailable(StrawberryConfig)
    include(StrawberryConfig)
endif ()


if (NOT TARGET StrawberryCore)
    option(STRAWBERRY_CORE_ENABLE_LOGGING_STACKTRACE "" OFF)
    option(STRAWBERRY_CORE_ENABLE_LOGGING_TIMESTAMPS "" OFF)


    FetchContent_Declare(FMT
            EXCLUDE_FROM_ALL
            GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
            GIT_TAG "11.1.4")
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


    option(TRACY_ENABLE "" OFF)
    option(TRACY_ON_DEMAND "" ${TRACY_ENABLE})
    FetchContent_Declare(
            tracy
            GIT_REPOSITORY "https://github.com/wolfpld/tracy.git"
            GIT_TAG "v0.11.1"
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(tracy)
    FetchContent_GetProperties(tracy
            SOURCE_DIR TRACY_SRC_DIR)


    new_strawberry_library(NAME StrawberryCore SOURCE
            src/Strawberry/Core/Assert.hpp
            src/Strawberry/Core/Error.cpp
            src/Strawberry/Core/Error.hpp
            src/Strawberry/Core/IO/Base64.cpp
            src/Strawberry/Core/IO/Base64.hpp
            src/Strawberry/Core/IO/Broadcaster.hpp
            src/Strawberry/Core/IO/ByteBuffer.hpp
            src/Strawberry/Core/IO/CallbackChannelReceiver.hpp
            src/Strawberry/Core/IO/CallbackReceiver.hpp
            src/Strawberry/Core/IO/ChannelBroadcaster.hpp
            src/Strawberry/Core/IO/ChannelReceiver.hpp
            src/Strawberry/Core/IO/DynamicByteBuffer.cpp
            src/Strawberry/Core/IO/DynamicByteBuffer.hpp
            src/Strawberry/Core/IO/Endian.hpp
            src/Strawberry/Core/IO/Error.hpp
            src/Strawberry/Core/IO/Logging.cpp
            src/Strawberry/Core/IO/Receiver.hpp
            src/Strawberry/Core/Markers.cpp
            src/Strawberry/Core/Markers.hpp
            src/Strawberry/Core/Math/Clamped.hpp
            src/Strawberry/Core/Math/Math.hpp
            src/Strawberry/Core/Math/Math.inl
            src/Strawberry/Core/Math/Matrix.hpp
            src/Strawberry/Core/Math/Noise.cpp
            src/Strawberry/Core/Math/Noise.hpp
            src/Strawberry/Core/Math/Periodic.hpp
            src/Strawberry/Core/Math/Rational.hpp
            src/Strawberry/Core/Math/Transformations.hpp
            src/Strawberry/Core/Math/Units.hpp
            src/Strawberry/Core/Math/Vector.hpp
            src/Strawberry/Core/Process.cpp
            src/Strawberry/Core/Process.hpp
            src/Strawberry/Core/Sync/ConditionVariable.cpp
            src/Strawberry/Core/Sync/ConditionVariable.hpp
            src/Strawberry/Core/Sync/Mutex.hpp
            src/Strawberry/Core/Sync/ProducerConsumerQueue.hpp
            src/Strawberry/Core/Sync/Spinlock.cpp
            src/Strawberry/Core/Sync/Spinlock.hpp
            src/Strawberry/Core/Thread/RepeatingTask.hpp
            src/Strawberry/Core/Thread/ThreadPool.cpp
            src/Strawberry/Core/Thread/ThreadPool.hpp
            src/Strawberry/Core/Thread/Worker.cpp
            src/Strawberry/Core/Thread/Worker.hpp
            src/Strawberry/Core/Traits.hpp
            src/Strawberry/Core/Types/CopyOnWrite.hpp
            src/Strawberry/Core/Types/Date.cpp
            src/Strawberry/Core/Types/Date.hpp
            src/Strawberry/Core/Types/Delayed.hpp
            src/Strawberry/Core/Types/DynamicValue.hpp
            src/Strawberry/Core/Types/Lazy.hpp
            src/Strawberry/Core/Types/Lazy.hpp
            src/Strawberry/Core/Types/NullValue.hpp
            src/Strawberry/Core/Types/Optional.hpp
            src/Strawberry/Core/Types/Overload.hpp
            src/Strawberry/Core/Types/ReflexivePointer.hpp
            src/Strawberry/Core/Types/Result.hpp
            src/Strawberry/Core/Types/TypeSet.hpp
            src/Strawberry/Core/Types/Uninitialised.hpp
            src/Strawberry/Core/Types/Variant.hpp
            src/Strawberry/Core/UTF.cpp
            src/Strawberry/Core/UTF.hpp
            src/Strawberry/Core/Util/Alloc.hpp
            src/Strawberry/Core/Util/IDPool.hpp
            src/Strawberry/Core/Util/Image.hpp
            src/Strawberry/Core/Util/Image.inl
            src/Strawberry/Core/Util/Ranges.hpp
            src/Strawberry/Core/Util/STBImplementations.cpp
            src/Strawberry/Core/Util/Strings.hpp
    )


    if (${STRAWBERRY_CORE_ENABLE_LOGGING_STACKTRACE})
        target_compile_definitions(StrawberryCore PUBLIC "STRAWBERRY_CORE_ENABLE_LOGGING_STACKTRACE=${STRAWBERRY_CORE_ENABLE_LOGGING_STACKTRACE}")
    endif ()

    if (${STRAWBERRY_CORE_ENABLE_LOGGING_TIMESTAMPS})
        target_compile_definitions(StrawberryCore PUBLIC "STRAWBERRY_CORE_ENABLE_LOGGING_TIMESTAMPS=${STRAWBERRY_CORE_ENABLE_LOGGING_TIMESTAMPS}")
    endif ()

    target_include_directories(StrawberryCore
            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
            PUBLIC ${STB_SOURCE_DIR})
    target_link_libraries(StrawberryCore PUBLIC nlohmann_json::nlohmann_json fmt::fmt-header-only)
    if (${STRAWBERRY_CORE_ENABLE_LOGGING_STACKTRACE})
        target_link_libraries(StrawberryCore PUBLIC stdc++exp)
    endif ()

    if (${TRACY_ENABLE})
        target_link_libraries(StrawberryCore PUBLIC TracyClient)
        if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
            target_compile_options(StrawberryCore PUBLIC "-fno-omit-frame-pointer")
        endif ()
    endif ()
    target_precompile_headers(StrawberryCore PUBLIC ${TRACY_SRC_DIR}/public/tracy/Tracy.hpp)


    new_strawberry_tests(NAME "StrawberryCore" TESTS
            test/Base64.cpp
            test/ChannelBroadcaster.cpp
            test/ClampedNumbers.cpp
            test/DynamicByteBuffer.cpp
            test/Matrices.cpp
            test/Noise.cpp
            test/Optional.cpp
            test/PeriodicNumbers.cpp
            test/ProducerConsumer.cpp
            test/TypeSets.cpp
            test/UTF.cpp
            test/Uninitialised.cpp
            test/Variants.cpp
            test/Vectors.cpp
    )


    # Setup LLDB for type summaries
    set(STRAWBERRY_CORE_TYPE_FORMATTERS ${CMAKE_CURRENT_SOURCE_DIR}/share/TypeRenderers.py)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/share/.lldbinit.template ${CMAKE_CURRENT_SOURCE_DIR}/.lldbinit)
endif ()


