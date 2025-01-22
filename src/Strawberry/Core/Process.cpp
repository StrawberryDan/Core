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
		std::string commandLine = fmt::format(R"("{}" {})", executable, FormatArguments(arguments));
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
			mResult->exitCode = GetExitCode();
		}
		return mResult.Value();
	}



	std::string Process::FormatArguments(const Arguments& arguments) noexcept
	{
		// Calculate the size of the final formatted string.
		// If there are not args, then this is zero;
		size_t formattedSize = 0;
		if (arguments.size() > 0) [[likely]]
		{
			// Get the sum of sizes of the arguments.
			formattedSize = std::ranges::fold_left(
				arguments | std::views::transform([](auto&& x) { return x.size(); }), 
				0, std::plus{});
			// Add bytes for 2 quote marks per argument and a space between each.
			formattedSize += 3 * arguments.size() - 1;
		}

		// Allocate the needed space and copy over each argument.
		std::string formattedArgs(formattedSize, '\0');
		std::string::iterator writer = formattedArgs.begin();
		for (auto&& [index, argument] : std::views::enumerate(arguments))
		{
			(*writer++) = '\"';
			writer = std::copy(argument.begin(), argument.end(), writer);
			(*writer++) = '\"';

			if (index < arguments.size() - 1) [[unlikely]]
			{
				(*writer++) = ' ';
			}
		}
		AssertEQ(writer, formattedArgs.end());

		return formattedArgs;
	}


	Process::ExitCode Process::GetExitCode() const
	{
#ifdef STRAWBERRY_TARGET_WINDOWS
		DWORD exitCode;
		BOOL status = GetExitCodeProcess(mProcessInformation.hProcess, &exitCode);

		if (status == 0) [[unlikely]]
		{
			switch (auto error = GetLastError())
			{
			default:
				Logging::Error("GetExitCodeProcess() failed with error code {}!", error);
				Core::Unreachable();
			}
		}

		return static_cast<int>(exitCode);
#endif // STRAWBERRY_TARGET_WINDOWS
	}
}
