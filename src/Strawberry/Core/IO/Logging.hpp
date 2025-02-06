#pragma once


#include "fmt/format.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


namespace Strawberry::Core
{
	class Logging
	{
	public:
		enum class Level
		{
			Trace,
			Debug,
			Info,
			Warning,
			Error,
		};


		static std::string LevelToString(Logging::Level logLevel);


		template<typename... Args>
		static void Log(Level level, fmt::format_string<Args...> message, Args&&... args)
		{
			// Return early if we are ignoring this log level.
			if (GetLevel() > level) return;

			// Create string
			std::string formatted;
			// Do as much at compile time as possible
			if consteval 
			{
				formatted = fmt::format(message, args...);
			}
			 else
			{
				formatted = fmt::format(fmt::runtime(message), args...);
			}

			// Put the string into the logging syntax.
			formatted = fmt::format("[{}]\t{}", LevelToString(level), formatted);
			LogRaw(level, formatted);
		}


		template<typename... Args>
		static void Trace(fmt::format_string<Args...> message, Args&&... args)
		{
			Log(Level::Trace, message, std::forward<Args>(args)...);
		}


		template<typename... Args>
		static void Debug(fmt::format_string<Args...> message, Args&&... args)
		{
			Log(Level::Debug, message, std::forward<Args>(args)...);
		}


		template<typename... Args>
		static void Info(fmt::format_string<Args...> message, Args&&... args)
		{
			Log(Level::Info, message, std::forward<Args>(args)...);
		}


		template<typename... Args>
		static void Warning(fmt::format_string<Args...> message, Args&&... args)
		{
			Log(Level::Warning, message, std::forward<Args>(args)...);
		}


		template<typename... Args>
		static void Error(fmt::format_string<Args...> message, Args&&... args)
		{
			Log(Level::Error, message, std::forward<Args>(args)...);
		}


		static Level GetLevel();
		static void  SetLevel(Level logLevel);
		static void  SetOutputFile(std::string filename);

	private:
		static void LogRaw(Level level, const std::string& message);
	};
} // namespace Strawberry::Core
