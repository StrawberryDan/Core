#pragma once



namespace Strawberry::Standard::Net
{
	enum class Error
	{
		DNSResolution,
		SocketCreation,
		AddressResolution,
		EstablishConnection,
		SSLAllocation,
		SSLHandshake,
	};
}