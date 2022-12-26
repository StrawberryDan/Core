#include "Standard/Log.hpp"

#include "Standard/Option.hpp"
#include <fstream>
#include <sstream>
#include <utility>


namespace Strawberry::Standard
{
	static Option<LogLevel>      sLogLevel   = {};
	static Option<std::ofstream> sOutputFile = {};



	std::string ToString(LogLevel logLevel)
	{
		switch (logLevel)
		{
			case LogLevel::Trace:
				return "TRACE";
			case LogLevel::Debug:
				return "DEBUG";
			case LogLevel::Info:
				return "INFO";
			case LogLevel::Warning:
				return "WARNING";
			case LogLevel::Error:
				return "ERROR";
		}
	}



	LogLevel GetLogLevel()
	{
		return sLogLevel.UnwrapOr(LogLevel::Warning);
	}



	void SetLogLevel(LogLevel logLevel)
	{
		sLogLevel = logLevel;
	}



	void SetOutputFile(std::string message)
	{
		sOutputFile.Emplace(std::move(message));
	}



	void LogRaw(std::string message)
	{
		std::cout << message << std::endl;
		if (sOutputFile)
		{
			*sOutputFile << message << std::endl;
		}
	}
}