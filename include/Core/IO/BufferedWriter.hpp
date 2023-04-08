#pragma once



#include "Core/IO/ByteBuffer.hpp"
#include "Core/IO/CircularDynamicByteBuffer.hpp"
#include "Core/Markers.hpp"
#include "Core/Mutex.hpp"
#include <concepts>
#include <memory>
#include <thread>



namespace Strawberry::Core::IO
{
	template <typename T> requires Read<T> && std::movable<T>
	class BufferedWriter
	{
	public:
		BufferedWriter(T&& source)
			: mRunning(std::make_unique<bool>(false))
			, mBuffer(std::make_unique<Mutex<Buffer>>())
			, mSource(std::make_unique<T>(std::move(source)))
		{
			StartThread();
		}

		BufferedWriter(const BufferedWriter& other)				= delete;
		BufferedWriter& operator=(const BufferedWriter& other)	= delete;
		BufferedWriter(BufferedWriter&& other)					= default;
		BufferedWriter& operator=(BufferedWriter&& other)		= default;



		~BufferedWriter()
		{
			StopThread();
		}



		inline Result<DynamicByteBuffer, Error> Read(size_t len) { if (mSource) return mSource->Read(len); else return Error::NoIO; }
        
		inline Result<size_t, Error> Write(const DynamicByteBuffer& bytes)
		{
			return mBuffer->Lock()->Write(bytes);
		}



	private:
		void StartThread()
		{
			*mRunning = true;
			if (!mThread)
			{
				mThread = std::make_unique<std::thread>([running = mRunning.get(), source = mSource.get(), buffer = mBuffer.get()]()
				{
					while (*running)
					{
						DynamicByteBuffer data;
						size_t len = 0;
						{
							auto locked = buffer->Lock();
							len = locked->Size();
							if (len > 0)
							{
								data = locked->Read(len).Unwrap();
							}
						}

						size_t bytesWritten = 0;
						while (source && bytesWritten < data.Size())
						{
							auto write = source->Write(
									{data.Data() + bytesWritten, data.Size() - bytesWritten});
							if (write)
							{
								bytesWritten += write.Unwrap();
							}
							else
							{
								Unreachable();
							}
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
		using Buffer = CircularDynamicByteBuffer;

		std::unique_ptr<bool>							mRunning;
		std::unique_ptr<std::thread>					mThread;
		std::unique_ptr<Mutex<Buffer>>					mBuffer;
		std::unique_ptr<T>								mSource;
	};
}
