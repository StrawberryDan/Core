#pragma once



#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "fmt/format.h"



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



	std::string ToString(LogLevel logLevel);



	LogLevel GetLogLevel();
	void     SetLogLevel(LogLevel logLevel);
	void     SetOutputFile(std::string filename);



	void LogRaw(std::string message);


	template <typename... Args>
	void Log(LogLevel level, std::string message, Args... args)
	{
		if (GetLogLevel() > level)
		{
			return;
		}

		std::string formatted = fmt::format(message, args...);

		std::stringstream out;
		out << "[" << ToString(level) << "]\t" << formatted;
		LogRaw(out.str());
	}

	template <typename... Args>
	void LogTrace(std::string message, Args... args)
	{
		Log(LogLevel::Trace, message, args...);
	}

	template <typename... Args>
	void LogDebug(std::string message, Args... args)
	{
		Log(LogLevel::Debug, message, args...);
	}

	template <typename... Args>
	void LogInfo(std::string message, Args... args)
	{
		Log(LogLevel::Info, message, args...);
	}

	template <typename... Args>
	void LogWarning(std::string message, Args... args)
	{
		Log(LogLevel::Warning, message, args...);
	}

	template <typename... Args>
	void LogError(std::string message, Args... args)
	{
		Log(LogLevel::Error, message, args...);
	}
}