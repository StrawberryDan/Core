#pragma once
//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#ifdef STRAWBERRY_TARGET_WINDOWS
#include <processthreadsapi.h>
#endif
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

	public:
		Process(const Executable& executable, const Arguments& arguments);
		~Process();


	private:
		PROCESS_INFORMATION mProcessInformation;
	};
}
