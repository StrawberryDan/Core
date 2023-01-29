#pragma once



#include <string>
#include "Core/Option.hpp"



namespace Strawberry::Core::Net::HTTP
{
	enum class Verb
	{
		POST,
		GET,
		PUT,
		PATCH,
		DEL,
	};



	std::string  ToString(Verb verb);
	Option<Verb> ParseVerb(const std::string& string);



	enum class Version
	{
		VERSION_1_0,
		VERSION_1_1,
		VERSION_2,
		VERSION_3
	};


	Option<Version> ParseVersion(const std::string& string);
	std::string     ToString(Version version);
}