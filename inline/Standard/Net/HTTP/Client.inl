#pragma once



#include <regex>
#include "Standard/Assert.hpp"
#include <iostream>
#include "fmt/core.h"



namespace Strawberry::Standard::Net::HTTP
{
	template<typename S> requires std::derived_from<S, Sockets::Socket>
	ClientImpl<S>::ClientImpl(const std::string& hostname, uint16_t port)
			: mSocket(hostname, port)
	{
	}



	template<typename S> requires std::derived_from<S, Sockets::Socket>
	void ClientImpl<S>::SendRequest(const Request& request)
	{
		std::string headerLine = fmt::format(
				"{} {} HTTP/{}\r\n",
				ToString(request.GetVerb()), request.GetURI(), ToString(request.GetVersion()));
		mSocket.WriteArray(headerLine.data(), headerLine.length()).Unwrap();
		for (const auto& [key, values]: *request.GetHeader())
		{
			for (const auto& value: values)
			{
				std::string formatted = fmt::format("{}: {}\r\n", key, value);
				mSocket.WriteArray(formatted.data(), formatted.length()).Unwrap();
			}
		}

		std::vector<char> blankLine = {'\r', '\n'};
		mSocket.WriteVector(blankLine).Unwrap();

		if (request.GetPayload())
		{
			if (std::holds_alternative<SimplePayload>(*request.GetPayload()))
			{
				const auto& payload = std::get<SimplePayload>(*request.GetPayload());
				if (payload.Size() > 0)
				{
					mSocket.WriteVector(*payload).Unwrap();
				}
			} else if (std::holds_alternative<ChunkedPayload>(*request.GetPayload()))
			{
				const auto& payload = std::get<ChunkedPayload>(*request.GetPayload());
				for (const auto& chunk: *payload)
				{
					mSocket.WriteVector(*chunk).Unwrap();
				}
			}
		}
	}



	template<typename S> requires std::derived_from<S, Sockets::Socket>
	Response ClientImpl<S>::Receive()
	{
		static const auto statusLinePattern = std::regex(R"(HTTP\/([^\s]+)\s+(\d{3})\s+([^\r]*)\r\n)");
		static const auto headerLinePattern = std::regex(R"(([^:]+)\s*:\s*([^\r]+)\r\n)");

		auto currentLine = ReadLine();
		std::smatch matchResults;
		auto matched = std::regex_match(currentLine, matchResults, statusLinePattern);
		Assert(matched);

		std::string version = matchResults[1],
				status = matchResults[2],
				statusText = matchResults[3];

		Response response(*ParseVersion(version), std::stoul(status), statusText);

		while (true)
		{
			currentLine = ReadLine();

			if (currentLine == "\r\n")
			{
				break;
			}
			else if (std::regex_match(currentLine, matchResults, headerLinePattern))
			{
				response.GetHeader().Add(matchResults[1], matchResults[2]);
			}
		}

		Payload payload;
		if (response.GetHeader().Contains("Transfer-Encoding"))
		{
			auto transferEncoding = response.GetHeader().Get("Transfer-Encoding");

			if (transferEncoding == "chunked")
			{
				payload = ReadChunkedPayload();
			}
			else
			{
				fmt::print(stderr, "Unsupported value for Transfer-Encoding: {}\n", transferEncoding);
			}
		}
		else if (response.GetHeader().Contains("Content-Length"))
		{
			SimplePayload simplePayload;
			unsigned long contentLength = std::stoul(response.GetHeader().Get("Content-Length"));
			if (contentLength > 0)
			{
				std::vector<uint8_t> data = mSocket.template ReadVector<uint8_t>(contentLength).Unwrap();
				simplePayload.Write(data.data(), data.size());
				payload = simplePayload;
			}
		}
		response.SetPayload(payload);

		return response;
	}



	template<typename S> requires std::derived_from<S, Sockets::Socket>
	ChunkedPayload ClientImpl<S>::ReadChunkedPayload()
	{
		std::string line;
		std::smatch matchResults;
		static const auto chunkSizeLine = std::regex(R"((\d+)\r\n)");

		ChunkedPayload payload;
		while (true)
		{
			line = this->ReadLine();
			if (line == "\r\n" || !std::regex_match(line, matchResults, chunkSizeLine))
			{
				break;
			}

			auto bytesToRead = std::stoul(matchResults[1], nullptr, 16);
			if (bytesToRead > 0)
			{
				ChunkedPayload::Chunk chunk(this->mSocket.template ReadVector<uint8_t>(bytesToRead).Unwrap());
				Assert(chunk.Size() == bytesToRead);
				payload.AddChunk(chunk);
			}
		}

		return payload;
	}



	template<typename S> requires std::derived_from<S, Sockets::Socket>
	std::string ClientImpl<S>::ReadLine()
	{
		std::string line;
		while (!line.ends_with("\r\n"))
		{
			line += mSocket.template ReadType<char>().Unwrap();
		}
		return line;
	}
}