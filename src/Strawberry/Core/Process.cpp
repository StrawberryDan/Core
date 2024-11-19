//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Process.hpp"

#include <filesystem>

#include "Strawberry/Core/IO/Logging.hpp"
#include <fmt/format.h>
#include <ranges>
#include <windows.h>


//======================================================================================================================
//		Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	Process::Process(const Executable& executable, const Arguments& arguments)
	{
		std::string commandLine = [&]()
		{
			const auto& quotedArgs = arguments | std::views::transform([](const std::string& x){return fmt::format("\"{}\"", x);});
			const auto& joinWithSpace = [](std::string&& a, std::string b) { return fmt::format("{} {}", a, b); };
			return fmt::format("\"{}\" {}", executable, std::ranges::fold_left_first(quotedArgs, joinWithSpace).value_or(""));
		}();

		Core::Logging::Info("Running command [{}]", commandLine);
		if (!CreateProcessA(
			/* application = */ nullptr,
			/* commandline = */ commandLine.empty() ? nullptr : commandLine.data(),
			/* processAttributes */ nullptr,
			/* threadAttributes */ nullptr,
			/* inheritHandles */ FALSE,
			/* creationFlags */ 0,
			/* environment */ nullptr,
			/* currentDirectory */ nullptr,
			/* startupInfo */ &mProcessStartupInfo,
			/* processInformation */ &mProcessInformation))
		{
			auto error = GetLastError();
			switch (error)
			{
				case ERROR_FILE_NOT_FOUND:
					Core::Logging::Error("Could not find file {}", executable);
					break;
				default:
					Core::Logging::Error("CreateProcessA failed! Error code: {}.", error);
					break;
			}
		}
	}


	Process::~Process()
	{
		WaitForSingleObject(mProcessInformation.hThread, INFINITE);
		CloseHandle(mProcessInformation.hThread);
		CloseHandle(mProcessInformation.hProcess);
	}


	Process::Result Process::Wait() noexcept
	{
		if (!mResult)
		{
			WaitForSingleObject(mProcessInformation.hThread, INFINITE);
			mResult.Emplace();
		}
		return mResult.Value();
	}
}
