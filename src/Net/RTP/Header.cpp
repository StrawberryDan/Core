#include "Standard/Net/RTP/Header.hpp"



#include "Standard/Endian.hpp"



namespace Strawberry::Standard::Net::RTP
{
	Header::Header(PayloadType payloadType, SSRC ssrc, SequenceNumber seq, Timestamp time)
		: mPayloadType(payloadType & 0b01111111)
		, mSSRC(ssrc)
		, mSequenceNumber(seq)
		, mTimestamp(time)
	{

	}



	ByteBuffer Header::AsBytes() const
	{
		ByteBuffer bytes;

		uint8_t byte = 0b10000000;
		bytes.Push(byte);

		byte = 0 | mPayloadType;
		bytes.Push(byte);

		auto seq = Standard::ToBigEndian(mSequenceNumber);
		bytes.Push(seq);

		auto time = Standard::ToBigEndian(mTimestamp);
		bytes.Push(time);

		auto ssrc = Standard::ToBigEndian(mSSRC);
		bytes.Push(ssrc);

		Assert(bytes.Size() == 12);
		return bytes;
	}
}