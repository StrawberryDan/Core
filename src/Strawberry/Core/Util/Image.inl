#pragma once
#include "Image.hpp"


namespace Strawberry::Core
{
	template <typename PixelType>
	Core::Result<Image<PixelType>, IO::Error> Image<PixelType>::FromFile(const std::filesystem::path& path) noexcept
	{
		int x, y, channelsInFile;
		auto bytes = stbi_load(path.string().c_str(), &x, &y, &channelsInFile, PixelType::Channels);

		if (bytes == 0)
		{
			return IO::Error::NotFound;
		}

		auto result = Image(x, y, Core::IO::DynamicByteBuffer(bytes, y * x * PixelType::Channels));
		stbi_image_free(bytes);
		return result;
	}


	template <typename PixelType>
	Image<PixelType>::Image(uint32_t width, uint32_t height, const IO::DynamicByteBuffer& bytes) noexcept
		: mSize(width, height)
		, mPixels(bytes.AsVector<PixelType>())
	{
		Core::AssertEQ(mSize[0] * mSize[1] * sizeof(PixelType), bytes.Size());
	}


	template <typename PixelType>
	PixelType Image<PixelType>::Read(uint32_t x, uint32_t y) const noexcept
	{
		return Read({x, y});
	}


	template <typename Pixel>
	typename Image<Pixel>::PixelType Image<Pixel>::Read(Math::Vec2u x) const noexcept
	{
		return mPixels[mSize.FlattenR(x)];
	}


	template <typename PixelType>
	void Image<PixelType>::Write(uint32_t x, uint32_t y, PixelType pixel) noexcept
	{
		Write({x, y}, pixel);
	}


	template <typename Pixel>
	void Image<Pixel>::Write(Math::Vec2u x, PixelType pixel) noexcept
	{
		mPixels[mSize.FlattenR(x)] = pixel;
	}


	template <typename PixelType>
	uint32_t Image<PixelType>::Width() const noexcept
	{
		return mSize[0];
	}


	template <typename PixelType>
	uint32_t Image<PixelType>::Height() const noexcept
	{
		return mSize[1];
	}


	template <typename Pixel>
	Math::Vec2u Image<Pixel>::Size() const noexcept
	{
		return mSize;
	}


	template <typename PixelType>
	PixelType* Image<PixelType>::Data() noexcept
	{
		return mPixels.data();
	}


	template <typename PixelType>
	const PixelType* Image<PixelType>::Data() const noexcept
	{
		return mPixels.data();
	}


	template <typename PixelType>
	void Image<PixelType>::Save(const std::filesystem::path& path, unsigned int quality) const noexcept
	{
		int x = Width(), y = Height();
		if (path.extension() == ".png")
		{
			auto result = stbi_write_png(
				path.string().c_str(),
				x, y,
				PixelType::Channels,
				reinterpret_cast<const void*>(mPixels.data()),
				x * sizeof(PixelType));
			Core::Assert(result != 0);
		}
		else if (path.extension() == ".bmp")
		{
			auto result = stbi_write_bmp(
				path.string().c_str(),
				x, y,
				PixelType::Channels,
				reinterpret_cast<const void*>(mPixels.data()));
			Core::Assert(result != 0);
		}
		else if (path.extension() == ".jpg")
		{
			auto result = stbi_write_jpg(
				path.string().c_str(),
				x, y,
				PixelType::Channels,
				reinterpret_cast<const void*>(mPixels.data()),
				quality);
			Core::Assert(result != 0);
		}
		else
		{
			Logging::Error("Could not save image! Unsupported type extension: {}", path.extension().string());
		}
	}


	template <typename Pixel>
	void Image<Pixel>::Blit(const Image& other, Math::Vec2u offset)
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
