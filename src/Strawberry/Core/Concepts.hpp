#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include <concepts>
#include <type_traits>

//======================================================================================================================
//  Concept Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	template<typename T>
	concept Dereferencable = requires(T t)
	{
		!std::is_void_v<decltype(*t)>;
	};

	template <typename T, typename Target>
	concept DecaysTo = std::same_as<std::decay_t<T>, std::decay_t<Target>>;
}
