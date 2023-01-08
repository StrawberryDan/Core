#pragma once



namespace Strawberry::Standard::IO
{
	enum class Error
	{
		Unknown,
		NotEnoughData,
		BufferOverflow,
		NoIO,
	};
}
