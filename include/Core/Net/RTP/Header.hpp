#pragma once


#include <cstdint>
#include <vector>
#include "Core/IO/DynamicByteBuffer.hpp"



namespace Strawberry::Core::Net::RTP
{
	class Header
	{
	public:
		using PayloadType    =  uint8_t;
		using SSRC           = uint32_t;
		using SequenceNumber = uint16_t;
		using Timestamp      = uint32_t;



	public:
		Header(PayloadType payloadType, SSRC ssrc, SequenceNumber seq, Timestamp time);



		IO::DynamicByteBuffer AsBytes() const;



	private:
		PayloadType    mPayloadType;
		SSRC           mSSRC;
		SequenceNumber mSequenceNumber;
		Timestamp      mTimestamp;
	};
}
