#pragma once
// Strawberry Core
#include "Strawberry/Core/Types/Result.hpp"
#include "Strawberry/Core/Error.hpp"
// Standard Library
#include <filesystem>
#include <ranges>
#include <string>
#include <vector>



#ifdef STRAWBERRY_TARGET_WINDOWS
	#include <windows.h>
#elifdef STRAWBERRY_TARGET_MAC
	#include <spawn.h>
#endif // STRAWBERRY_TARGET_WINDOWS


namespace Strawberry::Core
{
	class Process
	{
	public:
		static Result<Process, Error> Spawn(const std::filesystem::path& executable, std::vector<std::string> arguments);


		Process(const Process&) = delete;
		Process& operator=(const Process&) = delete;


		Process(Process&& other) noexcept;


		Process& operator=(Process&& x) noexcept;


		~Process();


		void Wait();

	private:
		Process() = default;


#ifdef STRAWBERRY_TARGET_WINDOWS
		HANDLE mProcess = nullptr;
		DWORD mProcessID = 0;
		HANDLE mThread = nullptr;
		DWORD mThreadID = 0;
#elifdef STRAWBERRY_TARGET_MAC
		pid_t mPid{};
#endif
	};
}
