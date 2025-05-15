#include "Process.hpp"


namespace Strawberry::Core
{
	Result<Process, Error> Process::Spawn(const std::filesystem::path& executable,
		const std::vector<std::string>& arguments)
	{
		if (!exists(executable))
		{
			return ErrorFileNotFound(executable);
		}


		auto argumentsString = std::ranges::fold_left(
			arguments | std::views::transform([](auto&& x) { return std::format("\"{}\"", x); }),
			executable.string(),
			[](auto&& x, auto&& y) { return fmt::format("{} {}", x, y); }
		);

#ifdef STRAWBERRY_TARGET_WINDOWS
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
#endif
	}


	Process::Process(Process&& other) noexcept
#ifdef STRAWBERRY_TARGET_WINDOWS
		: mProcess(std::exchange(other.mProcess, nullptr))
		, mProcessID(std::exchange(other.mProcessID, 0))
		, mThread(std::exchange(other.mThread, nullptr))
		, mThreadID(std::exchange(other.mThreadID, 0))
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
#endif
	}


	void Process::Wait()
	{
#ifdef STRAWBERRY_TARGET_WINDOWS
		WaitForSingleObject(mProcess, INFINITE);
#endif
	}
}
