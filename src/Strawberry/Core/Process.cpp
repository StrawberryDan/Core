#include "Process.hpp"

#ifdef STRAWBERRY_TARGET_MAC
	#include <sys/wait.h>

	extern char** environ;
#endif


namespace Strawberry::Core
{
	Result<Process, Error> Process::Spawn(const std::filesystem::path& executable,
		std::vector<std::string> arguments)
	{
		if (!exists(executable))
		{
			return ErrorFileNotFound(executable);
		}

#ifdef STRAWBERRY_TARGET_WINDOWS
		std::string argumentsString = std::ranges::fold_left(
			arguments | std::views::transform([](auto&& x) { return std::format("\"{}\"", x); }),
			executable.string(),
			[](auto&& x, auto&& y) { return fmt::format("{} {}", x, y); }
		);

		STARTUPINFOA startupInfo
		{
			.cb = sizeof(STARTUPINFOA)
		};
		PROCESS_INFORMATION processInfo {};

		auto result = CreateProcessA(
			executable.string().c_str(),
			argumentsString.data(),
			NULL,
			NULL,
			FALSE,
			CREATE_NO_WINDOW,
			NULL,
			NULL,
			&startupInfo,
			&processInfo
		);

		if (result == TRUE)
		{
			Process process;
			process.mProcess = processInfo.hProcess;
			process.mProcessID = processInfo.dwProcessId;
			process.mThread = processInfo.hThread;
			process.mThreadID = processInfo.dwThreadId;
			return process;
		}

		return ErrorSystem();
#elifdef STRAWBERRY_TARGET_MAC
		std::vector<char*> argumentsArray =
			arguments
			| std::views::transform([](auto&& x) { return x.data(); })
			| std::ranges::to<std::vector>();
		pid_t pid{};

		const char* path = executable.c_str();

		int result = -1;
		AssertNEQ(executable.is_absolute(), executable.is_relative());
		if (executable.is_absolute())
		{
			result = posix_spawn(&pid, path, nullptr, nullptr, argumentsArray.data(), environ);
		}
		else
		{
			result = posix_spawnp(&pid, path, nullptr, nullptr, argumentsArray.data(), environ);
		}

		switch (result)
		{
			case 0:
				break;
			default:
				Core::DebugBreak();
		}

		Process process;
		process.mPid = pid;
		return process;
#else
	#error "No Implementation for Strawberry::Core::Process::Spawn!"
#endif
	}


	Process::Process(Process&& other) noexcept
#ifdef STRAWBERRY_TARGET_WINDOWS
		: mProcess(std::exchange(other.mProcess, nullptr))
		, mProcessID(std::exchange(other.mProcessID, 0))
		, mThread(std::exchange(other.mThread, nullptr))
		, mThreadID(std::exchange(other.mThreadID, 0))
#elifdef STRAWBERRY_TARGET_MAC
#else
		#error "No Implementation for Strawberry::Core::Process::Spawn!"
#endif
	{}


	Process& Process::operator=(Process&& x) noexcept
	{
		if (&x != this)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(x));
		}

		return *this;
	}


	Process::~Process()
	{
#ifdef STRAWBERRY_TARGET_WINDOWS
		if (mThread) CloseHandle(mThread);
		if (mThread) CloseHandle(mProcess);
#elifdef STRAWBERRY_TARGET_MAC
		// Nothing required.
#else
		#error "No Implementation for Strawberry::Core::Process::Spawn!"
#endif
	}


	void Process::Wait()
	{
#ifdef STRAWBERRY_TARGET_WINDOWS
		WaitForSingleObject(mProcess, INFINITE);
#elifdef STRAWBERRY_TARGET_MAC
		int status = 0;
		pid_t result = waitpid(mPid, &status, 0);


#else
		#error "No Implementation for Strawberry::Core::Process::Spawn!"
#endif
	}
}
