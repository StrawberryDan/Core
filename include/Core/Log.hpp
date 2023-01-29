#pragma once



#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "fmt/format.h"



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



		template <typename... Args>
		static void Log(Level level, std::string message, Args... args)
		{
			if (GetLevel() > level)
			{
				return;
			}

			std::string formatted = fmt::format(message, args...);

			std::stringstream out;
			out << "[" << LevelToString(level) << "]\t" << formatted;
			LogRaw(out.str());
		}



		template <typename... Args>
		static void Trace(std::string message, Args... args)
		{
			Log(Level::Trace, message, args...);
		}



		template <typename... Args>
		static void Debug(std::string message, Args... args)
		{
			Log(Level::Debug, message, args...);
		}



		template <typename... Args>
		static void Info(std::string message, Args... args)
		{
			Log(Level::Info, message, args...);
		}



		template <typename... Args>
		static void Warning(std::string message, Args... args)
		{
			Log(Level::Warning, message, args...);
		}

		template <typename... Args>
		static void Error(std::string message, Args... args)
		{
			Log(Level::Error, message, args...);
		}



		static Level    GetLevel();
		static void     SetLevel(Level logLevel);
		static void     SetOutputFile(std::string filename);



	private:
		static void LogRaw(const std::string& message);
	};
}