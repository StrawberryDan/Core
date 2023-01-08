#pragma once



#include "Standard/IO/ByteBuffer.hpp"
#include "Standard/IO/CircularByteBuffer.hpp"
#include "Standard/IO/CircularDynamicByteBuffer.hpp"
#include "Standard/IO/Concepts.hpp"
#include "Standard/Mutex.hpp"
#include <concepts>
#include <memory>
#include <thread>



namespace Strawberry::Standard::IO
{
	template <typename T, size_t Size = 0> requires Read<T> && std::movable<T>
	class BufferedReader
	{
	public:
		BufferedReader(T&& source);
		BufferedReader(const BufferedReader& other) = delete;
		BufferedReader& operator=(const BufferedReader& other) = delete;
		BufferedReader(BufferedReader&& other);
		BufferedReader& operator=(BufferedReader&& other);
		~BufferedReader();



		Result<DynamicByteBuffer, Error> Read(size_t len);
		inline Result<size_t, Error> Write(const DynamicByteBuffer& bytes) { if (mSource) return mSource->Write(bytes); else return Error::NoIO; }



	private:
		using Buffer = std::conditional_t<Size == 0, CircularDynamicByteBuffer, CircularByteBuffer<Size>>;

		std::unique_ptr<bool>          mRunning;
		Option<std::thread>            mThread;
		std::unique_ptr<Mutex<Buffer>> mBuffer;
		std::unique_ptr<T>             mSource;
	};



	template<typename T, size_t Size> requires Read<T> && std::movable<T>
	BufferedReader<T, Size>::BufferedReader(T&& source)
		: mRunning(std::make_unique<bool>(true))
		, mThread()
		, mBuffer(std::make_unique<Mutex<Buffer>>())
		, mSource(std::make_unique<T>(std::forward<T>(source)))
	{
		mThread = std::thread([running = mRunning.get(), source = mSource.get(), buffer = mBuffer.get()]()
		{
			while (running)
			{
				auto len = buffer->Lock()->RemainingCapacity();
				auto read = source->Read(len);
				if (read)
				{
					buffer->Lock()->Write(*read).Unwrap();
				}
				else
				{
					break;
				}

				std::this_thread::yield();
			}
		});
	}



	template<typename T, size_t Size> requires Read<T> && std::movable<T>
	BufferedReader<T, Size>::BufferedReader(BufferedReader&& other)
		: mRunning(Take(other.mRunning))
		, mThread(Take(other.mThread))
		, mSource(Take(other.mSource))
		, mBuffer(Take(other.mBuffer))
	{

	}



	template<typename T, size_t Size> requires Read<T> && std::movable<T>
	BufferedReader<T, Size>& BufferedReader<T, Size>::operator=(BufferedReader&& other)
	{
		if (this != &other)
		{
			mRunning = Take(other.mRunning);
			mThread = Take(other.mThread);
			mSource = Take(other.mSource);
			mBuffer = Take(other.mBuffer);
		}

		return *this;
	}



	template<typename T, size_t Size> requires Read<T> && std::movable<T>
	BufferedReader<T, Size>::~BufferedReader()
	{
		if (mSource)
		{
			mSource.reset();
		}

		if (mRunning)
		{
			*mRunning = false;
		}

		if (mThread)
		{
			mThread->join();
		}
	}



	template<typename T, size_t Size> requires Read<T> && std::movable<T>
	Result<DynamicByteBuffer, Error> BufferedReader<T, Size>::Read(size_t len)
	{
		return mBuffer->Lock()->Read(len);
	}
}