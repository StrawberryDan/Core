#pragma once
#include "Types/Variant.hpp"
#include <filesystem>



namespace Strawberry::Core
{
	struct ErrorFileNotFound
	{
		std::filesystem::path filePath;
	};


	class ErrorSystem
	{
	public:
		ErrorSystem();

	private:
#if STRAWBERRY_TARGET_WINDOWS
		uint32_t mErrorCode;
#endif
	};

	using Error = Variant<
		ErrorFileNotFound,
		ErrorSystem>;
}
