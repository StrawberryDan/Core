#include "Strawberry/Core/Log.hpp"

#include "Strawberry/Core/Option.hpp"
#include <fstream>
#include <sstream>
#include <utility>


namespace Strawberry::Core
{
	static Option<Logging::Level>      sLogLevel   = {};
	static Option<std::ofstream>       sOutputFile = {};



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



	void Logging::LogRaw(const std::string& message)
	{
		std::cout << message << std::endl;
		if (sOutputFile)
		{
			*sOutputFile << message << std::endl;
		}
	}
}