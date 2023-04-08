#pragma once



#include "Core/IO/ByteBuffer.hpp"
#include "Core/IO/CircularByteBuffer.hpp"
#include "Core/IO/CircularDynamicByteBuffer.hpp"
#include "Core/IO/Concepts.hpp"
#include "Core/Mutex.hpp"
#include <concepts>
#include <memory>
#include <thread>



namespace Strawberry::Core::IO
{
	template <typename T, size_t Size = 0> requires Read<T> && std::movable<T>
	class BufferedReader
	{
	public:
		BufferedReader(T&& source)
			: mBlocking(true)
			, mRunning(std::make_unique<std::atomic<bool>>(false))
			, mBuffer(std::make_unique<Mutex<Buffer>>())
			, mSource(std::make_unique<T>(std::move(source)))
		{
			StartThread();
		}

		BufferedReader(const BufferedReader& other)				= delete;
		BufferedReader& operator=(const BufferedReader& other)	= delete;
		BufferedReader(BufferedReader&& other)					= default;
		BufferedReader& operator=(BufferedReader&& other)		= default;



		~BufferedReader()
		{
			StopThread();
		}



		Result<DynamicByteBuffer, Error> Read(size_t len)
		{
			while (true)
			{
				auto result = mBuffer->Lock()->Read(len);

				if (mBlocking && !result && result.Err() == IO::Error::WouldBlock)
				{
					std::this_thread::yield();
					continue;
				}

				return result;
			}
		}

		inline Result<size_t, Error> Write(const DynamicByteBuffer& bytes) { if (mSource) return mSource->Write(bytes); else return Error::NoIO; }



		inline bool IsBlocking() const { return mBlocking; }
		inline void SetBlocking(bool blocking) { mBlocking = blocking; }



	private:
		void StartThread()
		{
			*mRunning = true;
			if (!mThread)
			{
				mThread = std::make_unique<std::thread>([running = mRunning.get(), source = mSource.get(), buffer = mBuffer.get()]()
				{
					while (*running && source)
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
		}

		void StopThread()
		{
			if (mThread)
			{
				*mRunning = false;
				mSource.reset();
				mThread->join();
				mThread.reset();
			}
		}



	private:
		using Buffer = std::conditional_t<Size == 0, CircularByteBuffer<Size>, CircularDynamicByteBuffer>;

		bool								mBlocking;
		std::unique_ptr<std::atomic<bool>>	mRunning;
		std::unique_ptr<std::thread>		mThread;
		std::unique_ptr<Mutex<Buffer>>		mBuffer;
		std::unique_ptr<T>					mSource;
	};

}
