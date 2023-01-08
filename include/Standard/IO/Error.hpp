#pragma once



namespace Strawberry::Standard::IO
{
	enum class Error
	{
		Unknown,
		WouldBlock,
		BufferOverflow,
		NoIO,
	};
}
