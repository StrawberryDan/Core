#include "Standard/Net/Websocket/Message.hpp"



#include "Standard/Endian.hpp"
#include "Standard/Assert.hpp"



namespace Strawberry::Standard::Net::Websocket
{
	Message::Message(Message::Opcode opcode, Payload payload)
	    : mOpcode(opcode)
	    , mPayload(payload)
	{

	}



	Message::Message(const std::string& string)
	    : mOpcode(Opcode::Text)
	    , mPayload(string.data(), string.data() + string.size())
	{

	}



	Message::Message(std::vector<uint8_t> bytes)
	    : mOpcode(Opcode::Binary)
	    , mPayload(std::move(bytes))
	{

	}



	std::string Message::AsString() const
	{
	    return std::string(mPayload.data(), mPayload.data() + mPayload.size());
	}



	Result<nlohmann::json, std::string> Message::AsJSON() const
	{
	    nlohmann::json json;
	    switch (mOpcode)
	    {
	        case Opcode::Text:
	        case Opcode::Binary:
	        {
	            try
	            {
	                json = nlohmann::json::parse(mPayload.begin(), mPayload.end());
	            }
	            catch (std::exception& e)
	            {
	                return Result<nlohmann::json, std::string>::Err("Parse Error");
	            }
	            return Result<nlohmann::json, std::string>::Ok(std::forward<nlohmann::json>(json));
	        }

	        default:
	            return Result<nlohmann::json, std::string>::Err("Invalid Message Type");
	    }
	}



	uint16_t Message::GetCloseStatusCode() const
	{
	    uint16_t s = static_cast<uint16_t>(mPayload[0]) << 0 | static_cast<uint16_t>(mPayload[1]) << 8;
	    s = FromBigEndian(s);
	    return s;
	}



	void Message::Append(const Message& other)
	{
	    mPayload.insert(mPayload.end(), other.mPayload.begin(), other.mPayload.end());
	}
}