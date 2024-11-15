//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Process.hpp"
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
		STARTUPINFOA startupInfo{};
		const auto& quotedArgs = arguments | std::views::transform([](const std::string& x){return fmt::format("\"{}\"", x);});
		const auto& joinWithComma = [](std::string&& a, std::string b) { return fmt::format("{}, {}", a, b); };
		auto commandLine = std::ranges::fold_left_first(quotedArgs, joinWithComma).value_or("");
		CreateProcessA(
			/* application = */ executable.c_str(),
			/* commandline = */ commandLine.data(),
			/* processAttributes */ nullptr,
			/* threadAttributes */ nullptr,
			/* inheritHandles */ FALSE,
			/* creationFlags */ 0,
			/* environment */ nullptr,
			/* currentDirectory */ nullptr,
			/* startupInfo */ &startupInfo,
			/* processInformation */ &mProcessInformation);
	}


	Process::~Process()
	{
		WaitForSingleObject(mProcessInformation.hThread, INFINITE);
		CloseHandle(mProcessInformation.hThread);
		CloseHandle(mProcessInformation.hProcess);
	}
}
