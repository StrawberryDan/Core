#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Strawberry/Core/Util/Option.hpp"
// Standard Library
#include <concepts>
#include <variant>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	template<typename ...Types>
	class Variant
	{
	public:
		template<std::convertible_to<std::variant<Types...>> T>
		Variant(T data)
			: mData(std::move(data)) {}


		bool ContainsValue() const { return !mData.valueless_by_exception(); }


		template<typename T>
		bool IsType() const { return std::holds_alternative<T>(mData); }


		template<typename T>
		Core::Option<T> Value() const&
		{
			Core::Assert(ContainsValue());
			if (std::holds_alternative<T>(mData))
			{
				return std::get<T>(mData);
			}
			else
			{
				return Core::NullOpt;
			}
		}


		template<typename T>
		Core::Option<T> Value() &&
		{
			Core::Assert(ContainsValue());
			if (std::holds_alternative<T>(mData))
			{
				return std::get<T>(std::move(mData));
			}
			else
			{
				return Core::NullOpt;
			}
		}


	private:
		std::variant<Types...> mData;


	};
}