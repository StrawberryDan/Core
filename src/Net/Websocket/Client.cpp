#include "Strawberry/Core/Net/Websocket/Client.hpp"


namespace Strawberry::Core::Net::Websocket
{
	Result<WSClient, Error> WSClient::Connect(const std::string& host, const std::string& resource, uint16_t port)
	{
		HTTP::HTTPClient handshaker(host, port);
		HTTP::Request    upgradeRequest(HTTP::Verb::GET, resource);
		upgradeRequest.GetHeader().Add("Host", host);
		upgradeRequest.GetHeader().Add("Upgrade", "websocket");
		upgradeRequest.GetHeader().Add("Connection", "Upgrade");
		upgradeRequest.GetHeader().Add("Sec-WebSocket-Key", GenerateNonce());
		upgradeRequest.GetHeader().Add("Sec-WebSocket-Version", "13");
		handshaker.SendRequest(upgradeRequest);
		auto response = handshaker.Receive();
		if (response.GetStatus() != 101)
		{
			return Error::Refused;
		}


		WSClient client;
		client.mSocket = Core::Mutex(std::move(handshaker).TakeSocket());

		return std::move(client);
	}


	Result<WSSClient, Error> WSSClient::Connect(const std::string& host, const std::string& resource, uint16_t port)
	{
		HTTP::HTTPSClient handshaker(host, port);
		HTTP::Request     upgradeRequest(HTTP::Verb::GET, resource);
		upgradeRequest.GetHeader().Add("Host", host);
		upgradeRequest.GetHeader().Add("Upgrade", "websocket");
		upgradeRequest.GetHeader().Add("Connection", "Upgrade");
		upgradeRequest.GetHeader().Add("Sec-WebSocket-Key", GenerateNonce());
		upgradeRequest.GetHeader().Add("Sec-WebSocket-Version", "13");
		handshaker.SendRequest(upgradeRequest);
		auto response = handshaker.Receive();
		if (response.GetStatus() != 101)
		{
			return Error::Refused;
		}


		WSSClient client;
		client.mSocket = Core::Mutex(std::move(handshaker).TakeSocket());
		return std::move(client);
	}
} // namespace Strawberry::Core::Net::Websocket