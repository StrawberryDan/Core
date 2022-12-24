#include "Standard/Log.hpp"

#include "Standard/Option.hpp"
#include <fstream>
#include <sstream>
#include <utility>


namespace Strawberry::Standard
{
	static Option<LogLevel>      sLogLevel   = {};
	static Option<std::ofstream> sOutputFile = {};



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



	void Log(LogLevel level, std::string message)
	{
		if (GetLogLevel() > level)
		{
			return;
		}

		std::string logLevelStr;
		switch (level)
		{
			case LogLevel::Trace:
				logLevelStr = "TRACE";
				break;
			case LogLevel::Debug:
				logLevelStr = "DEBUG";
				break;
			case LogLevel::Info:
				logLevelStr = "INFO";
				break;
			case LogLevel::Warning:
				logLevelStr = "WARNING";
				break;
				logLevelStr = "ERROR";
			case LogLevel::Error:
				break;
		}

		std::stringstream out;
		out << "[" << logLevelStr  << "]\t" << message;

		std::cout << out.str() << std::endl;
		if (sOutputFile)
		{
			*sOutputFile << out.str() << std::endl;
		}
	}



	void LogTrace(std::string message)
	{
		Log(LogLevel::Trace, std::move(message));
	}



	void LogDebug(std::string message)
	{
		Log(LogLevel::Debug, std::move(message));
	}



	void LogInfo(std::string message)
	{
		Log(LogLevel::Info, std::move(message));
	}



	void LogWarning(std::string message)
	{
		Log(LogLevel::Warning, std::move(message));
	}



	void LogError(std::string message)
	{
		Log(LogLevel::Error, std::move(message));
	}
}