#pragma once



#include "Constants.hpp"
#include "Header.hpp"
#include "Payload.hpp"



namespace Strawberry::Standard::Net::HTTP
{
	class Response
	{
	public:
		Response(Version mVersion, unsigned int mStatus, std::string mStatusText);



		inline const Version& GetVersion() const
		{ return mVersion; }



		inline const unsigned int& GetStatus() const
		{ return mStatus; }



		inline const std::string& GetStatusText() const
		{ return mStatusText; }



		inline const Header& GetHeader() const
		{ return mHeader; }



		inline Header& GetHeader()
		{ return mHeader; }



		inline const Payload& GetPayload() const
		{ return mPayload; }



		inline void SetPayload(const Payload& payload)
		{ mPayload = payload; }



	private:
		Version      mVersion;
		unsigned int mStatus;
		std::string  mStatusText;
		Header   mHeader;
		Payload      mPayload;
	};
}