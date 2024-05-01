#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
// STB
#include "stb_image.h"


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::Util
{
	template<typename T>
	struct PixelChannelCount {};


	struct PixelRGB
	{
		uint8_t r, g, b;
	};

	template <>
	struct PixelChannelCount<PixelRGB> : public std::integral_constant<uint32_t, 3>
	{};


	struct PixelRGBA
	{
		uint8_t r, g, b, a;
	};

	template <>
	struct PixelChannelCount<PixelRGBA> : public std::integral_constant<uint32_t, 3>
	{};


	template <typename PixelType>
	class Image
	{
	public:
		static Core::Result<Image, IO::Error> FromFile(const std::filesystem::path& path) noexcept;


		Image(uint32_t width, uint32_t height, IO::DynamicByteBuffer bytes) noexcept;


		PixelType Read(uint32_t x, uint32_t y) const noexcept;
		void      Write(uint32_t x, uint32_t y, PixelType pixel) noexcept;


		uint32_t Width() const noexcept;
		uint32_t Height() const noexcept;


		PixelType* Data() noexcept;
		const PixelType* Data() const noexcept;


	private:
		uint32_t mWidth;
		uint32_t mHeight;
		IO::DynamicByteBuffer mBytes;
	};


	template<typename PixelType>
	Core::Result<Image<PixelType>, IO::Error> Image<PixelType>::FromFile(const std::filesystem::path& path) noexcept
	{
		int x, y, channelsInFile;
		auto bytes = stbi_load(path.c_str(), &x, &y, &channelsInFile, PixelChannelCount<PixelType>());

		if (bytes == 0)
		{
			return IO::Error::NotFound;
		}

		auto result = Image(x, y, Core::IO::DynamicByteBuffer(bytes, y * x * PixelChannelCount<PixelType>()));
		stbi_image_free(bytes);
		return result;
	}


	template<typename PixelType>
	Image<PixelType>::Image(uint32_t width, uint32_t height, IO::DynamicByteBuffer bytes) noexcept
			: mWidth(width)
			, mHeight(height)
			, mBytes(std::move(bytes))
	{
		Core::AssertEQ(mWidth * mHeight * sizeof(PixelType), bytes.Size());
	}


	template<typename PixelType>
	PixelType Image<PixelType>::Read(uint32_t x, uint32_t y) const noexcept
	{
		const size_t stride = sizeof(PixelType) * mWidth;
		return *reinterpret_cast<PixelType*>(mBytes.Data() + y * stride + x * sizeof(PixelType));
	}


	template<typename PixelType>
	void Image<PixelType>::Write(uint32_t x, uint32_t y, PixelType pixel) noexcept
	{
		const size_t stride = sizeof(PixelType) * mWidth;
		*reinterpret_cast<PixelType*>(mBytes.Data() + y * stride + x * sizeof(PixelType)) = pixel;
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
		return reinterpret_cast<PixelType*>(mBytes.Data());
	}


	template<typename PixelType>
	const PixelType* Image<PixelType>::Data() const noexcept
	{
		return reinterpret_cast<PixelType*>(mBytes.Data());
	}
}