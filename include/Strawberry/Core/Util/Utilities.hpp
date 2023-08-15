#pragma once


#include <string>
#include <type_traits>


namespace Strawberry::Core
{
	template <typename T>
	requires (!std::is_pointer_v<T> && std::is_move_assignable_v<T>)
	inline constexpr T Take(T& origin)
	{
		T result = std::move(origin);
		if constexpr (std::is_fundamental_v<T>)
		{
			origin = 0;
		}
		return std::move(result);
	}


	template <typename T>
	inline constexpr T* Take(T*& origin)
	{
		T* result = origin;
		origin    = nullptr;
		return result;
	}


	template <typename T, typename R>
	inline constexpr T Replace(T& origin, R replacement)
		requires (std::is_move_assignable_v<T> && std::is_convertible_v<R, T>)
	{
		T result = std::move(origin);
		origin   = replacement;
		return result;
	}


	inline std::string ToUppercase(const std::string& str)
	{
		std::string uppercase;
		uppercase.reserve(str.size());
		for (auto c : str)
		{
			uppercase.push_back(static_cast<char>(std::toupper(c)));
		}
		return uppercase;
	}


	inline std::string ToLowercase(const std::string& str)
	{
		std::string lowercase;
		lowercase.reserve(str.size());
		for (auto c : str)
		{
			lowercase.push_back(static_cast<char>(std::tolower(c)));
		}
		return lowercase;
	}
} // namespace Strawberry::Core