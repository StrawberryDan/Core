#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Optional.hpp"
// Standard Library
#include <concepts>
#include <variant>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
    template<typename... Types>
    class Variant;


    template<typename V, typename... T>
    struct AddVariantTypes {};


    template<typename... VTs, typename... Ts>
    struct AddVariantTypes<Variant<VTs...>, Ts...>
    {
        using Type = Variant<VTs..., Ts...>;
    };


    template<typename V1, typename V2>
    struct MergeVariantTypes {};


    template<typename... Ts1, typename... Ts2>
    struct MergeVariantTypes<Variant<Ts1...>, Variant<Ts2...> >
    {
        using Type = Variant<Ts1..., Ts2...>;
    };


    template<typename T, typename... Ts>
    struct IsInVariant
            : public std::false_type {};


    template<typename T, typename... Ts> requires (std::same_as<T, Ts> || ...)
    struct IsInVariant<T, Variant<Ts...> >
            : public std::true_type {};


    template<typename... Types>
    class Variant
    {
        public:
            template<typename... Ts>
            using AddTypes = typename AddVariantTypes<Variant<Types...>, Ts...>::Type;

            template<typename... Ts>
            using MergeTypes = typename MergeVariantTypes<Variant<Types...>, Variant<Ts...> >::Type;


            template<std::convertible_to<std::variant<Types...> > T>
            Variant(T data)
                : mData(std::move(data)) {}


            [[nodiscard]] bool ContainsValue() const
            {
                return !mData.valueless_by_exception();
            }


            template<typename T>
            [[nodiscard]] bool IsType() const
            {
                return std::holds_alternative<T>(mData);
            }


            template<typename T>
            Core::Optional<T> Take()
            {
                Core::Assert(ContainsValue());
                if (std::holds_alternative<T>(mData))
                {
                    return std::move(std::get<T>(std::move(mData)));
                }
                else
                {
                    return Core::NullOpt;
                }
            }


            template<typename T> requires (std::copyable<T>)
            Core::Optional<T> Value() const
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
            T& Ref()
            {
                return std::get<T>(mData);
            }


            template<typename T>
            const T& Ref() const
            {
                return std::get<T>(mData);
            }


            template<typename T>
            Core::Optional<T*> Ptr()
            {
                Core::Assert(ContainsValue() && IsType<T>());
                return std::get_if<T>(&mData);
            }


            template<typename T>
            Core::Optional<const T*> Ptr() const &
            {
                Core::Assert(ContainsValue() && IsType<T>());
                return static_cast<const T*>(std::get_if<T>(&mData));
            }


            template<typename T> requires (IsInVariant<T, Variant>::value)
            bool operator==(const Variant& other) const
            {
                return IsType<T>() && Ref<T>() == other;
            }


            template<typename T> requires (IsInVariant<T, Variant>::value)
            bool operator!=(const Variant& other) const
            {
                return !IsType<T>() || Ref<T>() != other;
            }

        private:
            std::variant<Types...> mData;
    };
} // namespace Strawberry::Core
