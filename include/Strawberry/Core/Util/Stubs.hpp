#pragma once


#include <string>
#include <type_traits>


namespace Strawberry::Core
{
	inline std::string ToUppercase(const std::string& str)
	{
		std::string uppercase;
		uppercase.reserve(str.size());
		for (auto c : str) { uppercase.push_back(static_cast<char>(std::toupper(c))); }
		return uppercase;
	}


	inline std::string ToLowercase(const std::string& str)
	{
		std::string lowercase;
		lowercase.reserve(str.size());
		for (auto c : str) { lowercase.push_back(static_cast<char>(std::tolower(c))); }
		return lowercase;
	}
} // namespace Strawberry::Core