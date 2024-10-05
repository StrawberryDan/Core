#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
// STB
#include <Strawberry/Core/IO/Logging.hpp>

#include "stb_image.h"
#include "stb_image_write.h"


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	template<typename T, size_t D>
	struct Pixel
	{
	public:
		using Type                       = T;
		static constexpr size_t Channels = D;


		Pixel()
			: channels{T{0}} {}


		template<typename... Args> requires (sizeof...(Args) == D && (std::convertible_to<Args, T> && ...))
		Pixel(Args&&... args)
			: channels{std::forward<Args>(args)...} {}


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


	template<typename Pixel>
	class Image
	{
	public:
		using PixelType = Pixel;


		static Core::Result<Image, IO::Error> FromFile(const std::filesystem::path& path) noexcept;


		Image()
			: mWidth(0)
			, mHeight(0)
			, mPixels() {}


		Image(Core::Math::Vec2u size, PixelType pixel = PixelType{})
			: mWidth(size[0])
			, mHeight(size[0])
			, mPixels(Width() * Height(), pixel) {}


		Image(uint32_t width, uint32_t height, const IO::DynamicByteBuffer& bytes) noexcept;


		PixelType Read(uint32_t x, uint32_t y) const noexcept;
		void      Write(uint32_t x, uint32_t y, PixelType pixel) noexcept;


		uint32_t Width() const noexcept;
		uint32_t Height() const noexcept;


		PixelType*       Data() noexcept;
		const PixelType* Data() const noexcept;


		void Save(const std::filesystem::path& path, unsigned int quality = 100) const noexcept;


		void Blit(const Image& other, Core::Math::Vec2u offset = {0, 0});

	private:
		uint32_t           mWidth;
		uint32_t           mHeight;
		std::vector<Pixel> mPixels;
	};


	template<typename PixelType>
	Core::Result<Image<PixelType>, IO::Error> Image<PixelType>::FromFile(const std::filesystem::path& path) noexcept
	{
		int  x, y, channelsInFile;
		auto bytes = stbi_load(path.string().c_str(), &x, &y, &channelsInFile, PixelType::Channels);

		if (bytes == 0)
		{
			return IO::Error::NotFound;
		}

		auto result = Image(x, y, Core::IO::DynamicByteBuffer(bytes, y * x * PixelType::Channels));
		stbi_image_free(bytes);
		return result;
	}


	template<typename PixelType>
	Image<PixelType>::Image(uint32_t width, uint32_t height, const IO::DynamicByteBuffer& bytes) noexcept
		: mWidth(width)
		, mHeight(height)
		, mPixels(bytes.AsVector<PixelType>())
	{
		Core::AssertEQ(mWidth * mHeight * sizeof(PixelType), bytes.Size());
	}


	template<typename PixelType>
	PixelType Image<PixelType>::Read(uint32_t x, uint32_t y) const noexcept
	{
		return mPixels[y * mWidth + x];
	}


	template<typename PixelType>
	void Image<PixelType>::Write(uint32_t x, uint32_t y, PixelType pixel) noexcept
	{
		mPixels[y * mWidth + x] = pixel;
	}


	template<typename PixelType>
	uint32_t Image<PixelType>::Width() const noexcept
	{
		return mWidth;
	}


	template<typename PixelType>
	uint32_t Image<PixelType>::Height() const noexcept
	{
		return mHeight;
	}


	template<typename PixelType>
	PixelType* Image<PixelType>::Data() noexcept
	{
		return mPixels.data();
	}


	template<typename PixelType>
	const PixelType* Image<PixelType>::Data() const noexcept
	{
		return mPixels.data();
	}


	template<typename PixelType>
	void Image<PixelType>::Save(const std::filesystem::path& path, unsigned int quality) const noexcept
	{
		if (path.extension() == ".png")
		{
			int  x      = mWidth, y = mHeight;
			auto result = stbi_write_png(path.string().c_str(), x, y, PixelType::Channels, reinterpret_cast<const void*>(mPixels.data()),
			                             x * sizeof(PixelType));
			Core::Assert(result != 0);
		}
		else if (path.extension() == ".bmp")
		{
			int  x      = mWidth, y = mHeight;
			auto result = stbi_write_bmp(path.string().c_str(), x, y, PixelType::Channels, reinterpret_cast<const void*>(mPixels.data()));
			Core::Assert(result != 0);
		}
		else if (path.extension() == ".jpg")
		{
			int  x      = mWidth, y = mHeight;
			auto result = stbi_write_jpg(path.string().c_str(), x, y, PixelType::Channels, reinterpret_cast<const void*>(mPixels.data()), quality);
			Core::Assert(result != 0);
		}
		else
		{
			Core::Logging::Error("Unsupported type extension: {}", path.extension().string());
			Core::DebugBreak();
		}
	}


	template<typename Pixel>
	void Image<Pixel>::Blit(const Image& other, Core::Math::Vec2u offset)
	{
		for (unsigned y = 0; y < other.Height(); ++y)
		{
			for (unsigned x = 0; x < other.Width(); ++x)
			{
				Write(x + offset[0], y + offset[1], other.Read(x, y));
			}
		}
	}
}
