#pragma once
#include <vector>
#include <ranges>


namespace Strawberry::Core
{
    class Chunk
        : public std::ranges::range_adaptor_closure<Chunk>
    {
    public:
        Chunk(unsigned int chunkSize)
            : mChunkSize(chunkSize)
        {}


        template <std::ranges::range Range>
        constexpr auto operator()(Range range) const
        {
            std::vector<std::vector<std::ranges::range_value_t<Range>>> results;

            if constexpr (std::ranges::sized_range<Range>)
            {
                results.resize(CeilDiv(std::ranges::size(range), mChunkSize));
            }

            int i = 0;
            for (auto&& v : range)
            {
                if constexpr (!std::ranges::sized_range<Range>)
                {
                    if (results.size() <= i / mChunkSize)
                    {
                        results.emplace_back();
                    }
                }

                results[i / mChunkSize].emplace_back(v);
                i++;
            }

            return results;
        }

    private:
        unsigned mChunkSize;
    };


    class Enumerate
        : public std::ranges::range_adaptor_closure<Enumerate>
    {
    public:
        template <std::ranges::range Range>
        constexpr auto operator()(Range range) const
        {
            return std::views::zip(std::views::iota(0), std::move(range));
        }
    };
}