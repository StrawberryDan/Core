#pragma once


namespace Strawberry::Core::IO
{
	enum class Error
	{
		Unknown,
		Closed,
		EndOfFile,
		NotFound,
	};
}
