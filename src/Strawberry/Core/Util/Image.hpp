#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
// STB
#include "stb_image.h"
#include "stb_image_write.h"


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::Util
{
    template<typename T>
    struct PixelChannelCount {};


    struct PixelGreyscale
    {
        uint8_t g;
    };


    template<>
    struct PixelChannelCount<PixelGreyscale> : public std::integral_constant<uint32_t, 1> {};


    struct PixelRGB
    {
        uint8_t r, g, b;
    };


    template<>
    struct PixelChannelCount<PixelRGB> : public std::integral_constant<uint32_t, 3> {};


    struct PixelRGBA
    {
        uint8_t r, g, b, a;
    };


    template<>
    struct PixelChannelCount<PixelRGBA> : public std::integral_constant<uint32_t, 4> {};


    template<typename PixelType>
    class Image
    {
        public:
            static Core::Result<Image, IO::Error> FromFile(const std::filesystem::path& path) noexcept;


            Image(uint32_t width, uint32_t height, IO::DynamicByteBuffer bytes) noexcept;


            PixelType Read(uint32_t x, uint32_t y) const noexcept;
            void      Write(uint32_t x, uint32_t y, PixelType pixel) noexcept;


            uint32_t Width() const noexcept;
            uint32_t Height() const noexcept;


            PixelType*       Data() noexcept;
            const PixelType* Data() const noexcept;


            void Save(const std::filesystem::path& path, unsigned int quality = 100) const noexcept;

        private:
            uint32_t              mWidth;
            uint32_t              mHeight;
            IO::DynamicByteBuffer mBytes;
    };


    template<typename PixelType>
    Core::Result<Image<PixelType>, IO::Error> Image<PixelType>::FromFile(const std::filesystem::path& path) noexcept
    {
        int  x, y, channelsInFile;
        auto bytes = stbi_load(path.string().c_str(), &x, &y, &channelsInFile, PixelChannelCount<PixelType>());

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
        return *reinterpret_cast<const PixelType*>(mBytes.Data() + y * stride + x * sizeof(PixelType));
    }


    template<typename PixelType>
    void Image<PixelType>::Write(uint32_t x, uint32_t y, PixelType pixel) noexcept
    {
        const size_t stride                                                               = sizeof(PixelType) * mWidth;
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


    template<typename PixelType>
    void Image<PixelType>::Save(const std::filesystem::path& path, unsigned int quality) const noexcept
    {
        if (path.extension() == ".png")
        {
            int  x      = mWidth, y = mHeight;
            auto result = stbi_write_png(path.string().c_str(), x, y, PixelChannelCount<PixelType>(), mBytes.Data(), y * sizeof(PixelType));
            Core::Assert(result != 0);
        }
        else if (path.extension() == ".bmp")
        {
            int  x      = mWidth, y = mHeight;
            auto result = stbi_write_bmp(path.string().c_str(), x, y, PixelChannelCount<PixelType>(), mBytes.Data());
            Core::Assert(result != 0);
        }
        else if (path.extension() == ".jpg")
        {
            int  x      = mWidth, y = mHeight;
            auto result = stbi_write_jpg(path.string().c_str(), x, y, PixelChannelCount<PixelType>(), mBytes.Data(), quality);
            Core::Assert(result != 0);
        }
    }
}
