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
		static constexpr void Log(Level level, const std::string& message, Args&&... args)
		{
			// Return early if we are ignoring this log level.
			if (GetLevel() > level) return;

			// Create string
			std::string formatted;
			// Do as much at compile time as possible
			if consteval
			{
				formatted = fmt::format(message, std::forward<Args>(args)...);
			}
			else
			{
				formatted = fmt::format(fmt::runtime(message), std::forward<Args>(args)...);
			}

			// Put the string into the logging syntax.
			formatted = fmt::format("[{}]\t{}", LevelToString(level), formatted);
			LogRaw(level, formatted);
		}


		template<typename... Args>
		static void Trace(const std::string& message, Args&&... args)
		{
			Log(Level::Trace, message, std::forward<Args>(args)...);
		}


		template<typename... Args>
		static void Debug(const std::string& message, Args&&... args)
		{
			Log(Level::Debug, message, std::forward<Args>(args)...);
		}


		template<typename... Args>
		static void Info(const std::string& message, Args&&... args)
		{
			Log(Level::Info, message, std::forward<Args>(args)...);
		}


		template<typename... Args>
		static void Warning(const std::string& message, Args&&... args)
		{
			Log(Level::Warning, message, std::forward<Args>(args)...);
		}


		template<typename... Args>
		static void Error(const std::string& message, Args&&... args)
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
