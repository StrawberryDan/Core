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
}
