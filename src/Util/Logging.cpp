#include "Strawberry/Core/Util/Logging.hpp"

#include "Strawberry/Core/Util/Optional.hpp"
#include <fstream>
#include <sstream>
#include <utility>


namespace Strawberry::Core
{
	static Optional<Logging::Level> sLogLevel   = {};
	static Optional<std::ofstream>  sOutputFile = {};

	std::string Logging::LevelToString(Level logLevel)
	{
		switch (logLevel)
		{
			case Level::Trace:
				return "TRACE";
			case Level::Debug:
				return "DEBUG";
			case Level::Info:
				return "INFO";
			case Level::Warning:
				return "WARNING";
			case Level::Error:
				return "ERROR";
			default:
				Core::Unreachable();
		}
	}


	Logging::Level Logging::GetLevel()
	{
		return sLogLevel.UnwrapOr(Level::Trace);
	}


	void Logging::SetLevel(Level logLevel)
	{
		sLogLevel = logLevel;
	}


	void Logging::SetOutputFile(std::string message)
	{
		sOutputFile.Emplace(std::move(message));
	}


	void Logging::LogRaw(Level level, const std::string& message)
	{
		auto* outputStream = &std::cout;
		if (level == Level::Error) outputStream = &std::cerr;

		*outputStream << message << std::endl;
		if (sOutputFile) { *sOutputFile << message << std::endl; }
	}
} // namespace Strawberry::Core