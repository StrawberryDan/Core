#pragma once
//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#ifdef STRAWBERRY_TARGET_WINDOWS
#include <processthreadsapi.h>
#endif
#include "Strawberry/Core/Types/Optional.hpp"
#include <string>
#include <vector>

//======================================================================================================================
//		Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	class Process
	{
	public:
		using Executable = std::string;
		using Argument   = std::string;
		using Arguments  = std::vector<std::string>;
		using ExitCode   = int;

		struct Result
		{
			ExitCode exitCode;

			operator ExitCode() const noexcept { return exitCode; } 
		};

	public:
		Process(const Executable& executable)
			: Process(executable, {})
		{}

		Process(const Executable& executable, const Arguments& arguments);
		~Process();


		Result Wait() noexcept;


	private:
		static std::string FormatArguments(const Arguments& arguments) noexcept;
		ExitCode           GetExitCode() const;


	private:
#ifdef STRAWBERRY_TARGET_WINDOWS
		PROCESS_INFORMATION mProcessInformation{};
		STARTUPINFOA        mProcessStartupInfo{};
#endif // STRAWBERRY_TARGET_WINDOWS

		Core::Optional<Result> mResult;
	};
}
