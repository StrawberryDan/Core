#pragma once



#include <vector>
#include <cstdint>
#include <string>
#include <variant>
#include <optional>



#include "Standard/Result.hpp"
#include "nlohmann/json.hpp"



namespace Strawberry::Standard::Net::Websocket
{
	class Message
	{
	public:
	    enum class Opcode;
	    using Payload = std::vector<uint8_t>;

	public:
	    Message(Opcode opcode, Payload payload = {});
	    Message(const std::string& string);
	    Message(std::vector<uint8_t> bytes);


	    inline Opcode GetOpcode() const { return mOpcode; }
	    inline std::vector<uint8_t>         AsBytes() const { return mPayload; }
	    std::string                         AsString() const;
	    Result<nlohmann::json, std::string> AsJSON() const;
	    uint16_t GetCloseStatusCode() const;


	    void Append(const Message& other);


	private:
	    Opcode mOpcode;
	    Payload mPayload;
	};



	enum class Message::Opcode
	{
	    Continuation,
	    Text,
	    Binary,
	    Close,
	    Ping,
	    Pong,
	};
}