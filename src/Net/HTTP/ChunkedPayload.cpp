#include "Standard/Net/HTTP/ChunkedPayload.hpp"



namespace Strawberry::Standard::Net::HTTP
{
	void ChunkedPayload::AddChunk(const ChunkedPayload::Chunk& chunk)
	{
		mChunks.push_back(chunk);
	}



	const ChunkedPayload::Chunk& ChunkedPayload::Get(size_t index) const
	{
		return mChunks[index];
	}



	ChunkedPayload::Chunk& ChunkedPayload::Get(size_t index)
	{
		return mChunks[index];
	}



	size_t ChunkedPayload::Size() const
	{
		return mChunks.size();
	}
}