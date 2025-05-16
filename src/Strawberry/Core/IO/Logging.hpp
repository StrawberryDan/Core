#pragma once


#include "fmt/format.h"
#include <fstream>
#include <string>

#ifdef STRAWBERRY_CORE_ENABLE_LOGGING_STACKTRACE
	#include <stacktrace>
#endif

#ifdef STRAWBERRY_CORE_ENABLE_LOGGING_TIMESTAMPS
	#include <chrono>
	#include <fmt/chrono.h>
#endif


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
		static std::string LevelToString(Logging::Level logLevel);


#ifdef STRAWBERRY_CORE_ENABLE_LOGGING_STACKTRACE
		static std::stacktrace_entry GetCaller()
		{
			auto stacktrace = std::stacktrace::current();

			int i = 0;
			for (i = 2; i < stacktrace.size(); i++)
			{
				if (stacktrace.at(i).source_file().ends_with("Strawberry/Core/Assert.hpp"))
				{
					continue;
				}

				if (stacktrace.at(i).source_file().ends_with("Strawberry/Core/IO/Logging.hpp"))
				{
					continue;
				}

				return stacktrace.at(i);
			}

			return stacktrace.at(i);
		}
#endif


		template<typename... Args>
		static constexpr void Log(Level level, const std::string& message, Args&&... args)
		{
#ifdef STRAWBERRY_CORE_ENABLE_LOGGING_STACKTRACE
			std::stacktrace_entry caller = GetCaller();
#endif

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


#ifdef STRAWBERRY_CORE_ENABLE_LOGGING_STACKTRACE
			// Prepend source code location
			if (!caller.source_file().empty())
			{
				// Put the string into the logging syntax.
				formatted = fmt::format(
					"{}:{}\t{}",
					caller.source_file(),
					caller.source_line(),
					formatted);
			}
#endif


#ifdef STRAWBERRY_CORE_ENABLE_LOGGING_TIMESTAMPS
			formatted = fmt::format(
				"{}\t{}",
				std::chrono::system_clock::now(),
				formatted);
#endif

			// Put the string into the logging syntax.
			formatted = fmt::format(
				"[{}]\t{}",
				LevelToString(level),
				formatted);

			LogRaw(level, formatted);
		}


		static void LogRaw(Level level, const std::string& message);
	};
} // namespace Strawberry::Core
