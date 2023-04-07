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
		BufferedWriter(T&& source);
		BufferedWriter(const BufferedWriter& other) = delete;
		BufferedWriter& operator=(const BufferedWriter& other) = delete;
		BufferedWriter(BufferedWriter&& other);
		BufferedWriter& operator=(BufferedWriter&& other);
		~BufferedWriter();



		inline Result<DynamicByteBuffer, Error> Read(size_t len) { if (mSource) return mSource->Read(len); else return Error::NoIO; }
		inline Result<size_t, Error> Write(const DynamicByteBuffer& bytes);

	private:
		std::unique_ptr<bool>                             mRunning;
		Option<std::thread>                               mThread;
		std::unique_ptr<Mutex<CircularDynamicByteBuffer>> mBuffer;
		std::unique_ptr<T>                                mSource;
	};



	template<typename T> requires Read<T> && std::movable<T>
	BufferedWriter<T>::BufferedWriter(T&& source)
			: mRunning(std::make_unique<bool>(true))
			, mThread()
			, mBuffer(std::make_unique<Mutex<CircularDynamicByteBuffer>>())
			, mSource(std::make_unique<T>(std::forward<T>(source)))
	{
		mThread.Emplace([running = mRunning.get(), source = mSource.get(), buffer = mBuffer.get()]()
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
				while (bytesWritten < data.Size())
				{
					auto write = source->Write({data.Data() + bytesWritten, data.Size() - bytesWritten});
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



	template<typename T> requires Read<T> && std::movable<T>
	BufferedWriter<T>::BufferedWriter(BufferedWriter&& other)
			: mRunning(Take(other.mRunning))
			, mThread(Take(other.mThread))
			, mSource(Take(other.mSource))
			, mBuffer(Take(other.mBuffer))
	{

	}



	template<typename T> requires Read<T> && std::movable<T>
	BufferedWriter<T>& BufferedWriter<T>::operator=(BufferedWriter&& other)
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



	template<typename T> requires Read<T> && std::movable<T>
	BufferedWriter<T>::~BufferedWriter()
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



	template<typename T> requires Read<T> && std::movable<T>
	Result<size_t, Error> BufferedWriter<T>::Write(const DynamicByteBuffer& bytes)
	{
		return mBuffer->Lock()->Write(bytes);
	}
}