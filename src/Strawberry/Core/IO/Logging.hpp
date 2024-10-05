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
		static void Log(Level level, std::string message, Args... args)
		{
			if (GetLevel() > level) return;

			std::string formatted = fmt::format(fmt::runtime(message), args...);

			std::stringstream out;
			out << "[" << LevelToString(level) << "]\t" << formatted;
			LogRaw(level, out.str());
		}


		template<typename... Args>
		static void Trace(std::string message, Args&&... args)
		{
			Log(Level::Trace, message, std::forward<Args>(args)...);
		}


		template<typename... Args>
		static void Debug(std::string message, Args&&... args)
		{
			Log(Level::Debug, message, std::forward<Args>(args)...);
		}


		template<typename... Args>
		static void Info(std::string message, Args&&... args)
		{
			Log(Level::Info, message, std::forward<Args>(args)...);
		}


		template<typename... Args>
		static void Warning(std::string message, Args&&... args)
		{
			Log(Level::Warning, message, std::forward<Args>(args)...);
		}


		template<typename... Args>
		static void Error(std::string message, Args&&... args)
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
