#include "Strawberry/Core/UTF.hpp"
#include <string>


using namespace Strawberry::Core;


int main()
	{
		const char*    a   = "£";
		char32_t       a32 = ToUTF32(a).Unwrap();
		std::u32string a32str{a32};
		Assert(a32str == U"£");
		Assert(ToUTF8(a32).Unwrap() == a);

		std::u32string b  = U"兎田ぺこら";
		std::string    b8 = ToUTF8(b);
		Assert(ToUTF32(b8) == b);
	}