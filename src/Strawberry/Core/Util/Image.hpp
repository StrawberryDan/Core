#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
#include "Strawberry/Core/Thread/ThreadPool.hpp"
#include <Strawberry/Core/IO/Logging.hpp>
// STB
#include "stb_image.h"
#include "stb_image_write.h"


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	template <typename T, size_t D>
	struct Pixel
	{
	public:
		using Type = T;
		static constexpr size_t Channels = D;
		static constexpr size_t Size = sizeof(Type) * Channels;


		Pixel()
			: channels{T{0}}
		{
		}


		template <typename... Args> requires (sizeof...(Args) == D && (std::convertible_to<Args, T> && ...))
		Pixel(Args&&... args)
			: channels{static_cast<Type>(std::forward<Args>(args))...}
		{
		}


		auto&& operator[](this auto& self, size_t index)
		{
			return std::forward<decltype(self)>(self).channels[index];
		}


		T channels[D] = {0};
	};


	using PixelGreyscale    = Pixel<uint8_t, 1>;
	using PixelRGB          = Pixel<uint8_t, 3>;
	using PixelRGBA         = Pixel<uint8_t, 4>;
	using PixelF32Greyscale = Pixel<float, 1>;
	using PixelF32RGB       = Pixel<float, 3>;
	using PixelF32RGBA      = Pixel<float, 4>;


	struct ImageShadingContext
	{
		Math::Vector<unsigned int, 2> position;
	};


	template <typename Pixel>
	class Image
	{
	public:
		using PixelType = Pixel;


		static Core::Result<Image, IO::Error> FromFile(const std::filesystem::path& path) noexcept;


		Image()
			: mSize(0, 0)
			, mPixels()
		{}


		Image(Math::Vec2u size, PixelType pixel = PixelType{})
			: mSize(size)
			, mPixels(Width() * Height(), pixel)
		{}


		Image(uint32_t width, uint32_t height, const IO::DynamicByteBuffer& bytes) noexcept;


		PixelType Read(uint32_t x, uint32_t y) const noexcept;
		PixelType Read(Math::Vec2u x) const noexcept;
		void Write(uint32_t x, uint32_t y, PixelType pixel) noexcept;
		void Write(Math::Vec2u x, PixelType pixel) noexcept;


		template <typename ShadingFunction>
		void Shade(ShadingFunction shader)
		{
			for (int x = 0; x < Width(); x++)
			{
				for (int y = 0; y < Height(); y++)
				{
					ImageShadingContext context
						{
							.position { x, y }
						};
					Write(x, y, std::invoke(shader, context));
				}
			}
		}


		template <typename ShadingFunction>
		void Shade(ThreadPool& threadPool, ShadingFunction shader)
		{
			auto tasks = threadPool.QueueTasks(Size(), [&] (const auto& p) {
				ImageShadingContext context {
					.position = p,
				};

				Write(p[0], p[1], std::invoke(shader, context));
			}).Unwrap();

			for (auto& task : tasks)
			{
				task.second.wait();
			}
		}


		uint32_t Width() const noexcept;
		uint32_t Height() const noexcept;
		constexpr uint32_t PixelSize() const noexcept { return Pixel::Size; }
		Math::Vec2u Size() const noexcept;


		PixelType* Data() noexcept;
		const PixelType* Data() const noexcept;


		IO::DynamicByteBuffer AsBytes() const;


		void Save(const std::filesystem::path& path, unsigned int quality = 100) const noexcept;


		void Blit(const Image& other, Core::Math::Vec2u offset = {0, 0});


	private:
		Math::Vec2u mSize;
		std::vector<Pixel> mPixels;
	};
}


#include "Image.inl"
