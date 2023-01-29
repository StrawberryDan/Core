#pragma once



namespace Strawberry::Core::IO
{
	enum class Error
	{
		Unknown,
		WouldBlock,
		BufferOverflow,
		NoIO,
	};
}
