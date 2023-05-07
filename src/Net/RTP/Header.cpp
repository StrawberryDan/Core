#include "Strawberry/Core/Net/RTP/Header.hpp"



#include "Strawberry/Core/Endian.hpp"



namespace Strawberry::Core::Net::RTP
{
	Header::Header(PayloadType payloadType, SSRC ssrc, SequenceNumber seq, Timestamp time)
		: mPayloadType(payloadType & 0b01111111)
		, mSSRC(ssrc)
		, mSequenceNumber(seq)
		, mTimestamp(time)
	{

	}



	IO::DynamicByteBuffer Header::AsBytes() const
	{
		IO::DynamicByteBuffer bytes;

		uint8_t byte = 0b10000000;
		bytes.Push(byte);

		byte = 0 | mPayloadType;
		bytes.Push(byte);

		auto seq = Core::ToBigEndian(mSequenceNumber);
		bytes.Push(seq);

		auto time = Core::ToBigEndian(mTimestamp);
		bytes.Push(time);

		auto ssrc = Core::ToBigEndian(mSSRC);
		bytes.Push(ssrc);

		Assert(bytes.Size() == 12);
		return bytes;
	}
}