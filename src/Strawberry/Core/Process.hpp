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

		struct Result
		{

		};

	public:
		Process(const Executable& executable, const Arguments& arguments);
		~Process();


		Result Wait() noexcept;


	private:
#ifdef STRAWBERRY_TARGET_WINDOWS
		PROCESS_INFORMATION mProcessInformation{};
		STARTUPINFOA        mProcessStartupInfo{};
#endif // STRAWBERRY_TARGET_WINDOWS

		Core::Optional<Result> mResult;
	};
}
