#include "Strawberry/Core/Net/Websocket/Message.hpp"



#include "Strawberry/Core/Endian.hpp"
#include "Strawberry/Core/Assert.hpp"



namespace Strawberry::Core::Net::Websocket
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
	                return std::string("Parse Error");
	            }
	            return Result<nlohmann::json, std::string>::Ok(std::forward<nlohmann::json>(json));
	        }

	        default:
	            return std::string("Invalid Message Type");
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