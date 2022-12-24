#pragma once



#include <string>



namespace Strawberry::Standard
{
	enum class LogLevel
	{
		Trace,
		Debug,
		Info,
		Warning,
		Error,
	};



	LogLevel GetLogLevel();
	void     SetLogLevel(LogLevel logLevel);
	void     SetOutputFile(std::string filename);



	void Log(LogLevel level, std::string message);
	void LogTrace(std::string message);
	void LogDebug(std::string message);
	void LogInfo(std::string message);
	void LogWarning(std::string message);
	void LogError(std::string message);
}