#pragma once


#include <variant>
#include <optional>



#include "SimplePayload.hpp"
#include "ChunkedPayload.hpp"



namespace Strawberry::Standard::Net::HTTP
{
	using Payload = Option
    <
		std::variant
		<
				SimplePayload,
				ChunkedPayload
		>
	>;
}
