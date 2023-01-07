#pragma once



#include <regex>
#include "Standard/Assert.hpp"
#include <iostream>
#include "fmt/core.h"



namespace Strawberry::Standard::Net::HTTP
{
	template<typename S>
	ClientBase<S>::ClientBase(const std::string& hostname, uint16_t port)
			: mSocket(S::Connect(Endpoint::Resolve(hostname, port).Unwrap()).Unwrap())
	{
	}



	template<typename S>
	void ClientBase<S>::SendRequest(const Request& request)
	{
		std::string headerLine = fmt::format(
				"{} {} HTTP/{}\r\n",
				ToString(request.GetVerb()), request.GetURI(), ToString(request.GetVersion()));
		mSocket.Write({headerLine.data(), headerLine.length()}).Unwrap();
		for (const auto& [key, values]: *request.GetHeader())
		{
			for (const auto& value: values)
			{
				std::string formatted = fmt::format("{}: {}\r\n", key, value);
				mSocket.Write({formatted.data(), formatted.length()}).Unwrap();
			}
		}

		std::vector<char> blankLine = {'\r', '\n'};
		mSocket.Write({blankLine.data(), blankLine.size()}).Unwrap();

		if (request.GetPayload())
		{
			if (std::holds_alternative<SimplePayload>(*request.GetPayload()))
			{
				const auto& payload = std::get<SimplePayload>(*request.GetPayload());
				if (payload.Size() > 0)
				{
					mSocket.Write({payload.Data(), payload.Size()}).Unwrap();
				}
			} else if (std::holds_alternative<ChunkedPayload>(*request.GetPayload()))
			{
				const auto& payload = std::get<ChunkedPayload>(*request.GetPayload());
				for (const auto& chunk: *payload)
				{
					mSocket.Write({chunk.Data(), chunk.Size()}).Unwrap();
				}
			}
		}
	}



	template<typename S>
	Response ClientBase<S>::Receive()
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
				auto data = mSocket.Read(contentLength).Unwrap();
				simplePayload.Write(data.Data(), data.Size());
				payload = simplePayload;
			}
		}
		response.SetPayload(payload);

		return response;
	}



	template<typename S>
	ChunkedPayload ClientBase<S>::ReadChunkedPayload()
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
				ChunkedPayload::Chunk chunk(this->mSocket.Read(bytesToRead).Unwrap().AsVector());
				Assert(chunk.Size() == bytesToRead);
				payload.AddChunk(chunk);
			}
		}

		return payload;
	}



	template<typename S>
	std::string ClientBase<S>::ReadLine()
	{
		std::string line;
		while (!line.ends_with("\r\n"))
		{
			line += mSocket.Read(1).Unwrap().template Into<char>();
		}
		return line;
	}
}