#pragma once



#include <string>
#include "Strawberry/Core/Option.hpp"



namespace Strawberry::Core::Net::HTTP
{
	class Verb
	{
	public:
		enum __Enum
		{
			POST,
			GET,
			PUT,
			PATCH,
			DEL,
		};
	public:
		inline Verb(__Enum value) : mValue(value) {}
		operator __Enum() const { return mValue; }

		static Option<Verb> Parse(const std::string& string);
		std::string ToString() const;
	private:
		__Enum mValue;
	};



	class Version
	{
	public:
		enum __Enum
		{
			VERSION_1_0,
			VERSION_1_1,
			VERSION_2,
			VERSION_3
		};
	public:
		inline Version(__Enum value) : mValue(value) {}
		inline operator __Enum() const { return mValue; }

		static Option<Version> Parse(const std::string& string);
		std::string ToString() const;
	private:
		__Enum mValue;
	};
}