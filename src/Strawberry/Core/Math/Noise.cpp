#include "Noise.hpp"


namespace Strawberry::Core::Math
{
    LinearNoise::LinearNoise(uint32_t seed, float period)
            : mSeed(seed)
            , mPeriod(period)
    {}


    float LinearNoise::Noise(Core::Math::Vec2u position) const
    {
        const auto& [x, y] = position;

        uint32_t cantor = (x + y) * (x + y + 1) / 2 + y;

        std::mt19937 engine(mSeed xor cantor);

        const float randomSample = engine();
        constexpr float MAXIMUM = std::numeric_limits<uint32_t>::max();

        return randomSample / MAXIMUM;
    }


    float LinearNoise::operator()(Vec2f position) const noexcept
    {
        const auto& [x, y] = position;


        const float waveX = std::floor(x / mPeriod);
        const float waveY = std::floor(y / mPeriod);


        const float phaseX = std::fmod(x, mPeriod);
        const float phaseY = std::fmod(y, mPeriod);


        const float phaseRatioX = phaseX / mPeriod;
        const float phaseRatioY = phaseY / mPeriod;


        const float grid[2][2]
        {
            {Noise({waveX + 0, waveY + 0}), Noise({waveX + 1, waveY + 0})},
            {Noise({waveX + 0, waveY + 1}), Noise({waveX + 1, waveY + 1})}
        };


        const float a = std::lerp(grid[0][0], grid[0][1], phaseRatioX);
        const float b = std::lerp(grid[1][0], grid[1][1], phaseRatioX);
        const float c = std::lerp(a, b, phaseRatioY);

        return c;
    }
}