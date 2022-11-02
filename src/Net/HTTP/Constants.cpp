#include "Standard/Net/HTTP/Constants.hpp"



#include "Standard/Utilities.hpp"


#include <map>



namespace Strawberry::Standard::Net::HTTP
{
	std::string ToString(Verb verb)
	{
		switch (verb)
		{

			case Verb::POST:
				return "POST";
			case Verb::GET:
				return "GET";
			case Verb::PUT:
				return "PUT";
			case Verb::PATCH:
				return "PATCH";
			case Verb::DELETE:
				return "DELETE";
			default:
				std::abort();
		}
	}



	Option<Verb> ParseVerb(const std::string& string)
	{
		static const std::map<std::string, Verb> mapping =
				{
						{"POST",   Verb::POST},
						{"GET",    Verb::GET},
						{"PUT",    Verb::PUT},
						{"PATCH",  Verb::PATCH},
						{"DELETE", Verb::DELETE}
				};

		std::string upper = ToUppercase(string);
		if (mapping.contains(upper))
		{
			return mapping.at(upper);
		} else
		{
			return {};
		}
	}



	Option<Version> ParseVersion(const std::string& string)
	{
		static const std::map<std::string, Version> mapping =
				{
						{"1.0", Version::VERSION_1_0},
						{"1.1", Version::VERSION_1_1},
						{"2",   Version::VERSION_2},
						{"3",   Version::VERSION_3},
				};

		if (mapping.contains(string))
		{
			return mapping.at(string);
		} else
		{
			return {};
		}
	}



	std::string ToString(Version version)
	{
		switch (version)
		{
			case Version::VERSION_1_0:
				return "1.0";
			case Version::VERSION_1_1:
				return "1.1";
			case Version::VERSION_2:
				return "2";
			case Version::VERSION_3:
				return "3";
			default:
				std::abort();
		}
	}
}