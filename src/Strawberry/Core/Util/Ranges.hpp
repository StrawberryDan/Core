#pragma once
#include <ranges>


namespace Strawberry::Core
{
    class Chunk
        : public std::ranges::range_adaptor_closure<Chunk>
    {
    public:
        Chunk(unsigned chunkSize)
            : mSize(chunkSize)
        {}


        template <std::ranges::range Range>
        class ChunkedRange
            : public std::ranges::view_interface<ChunkedRange<Range>>
        {
            using Iter = std::ranges::iterator_t<Range>;


            ChunkedRange(Range range)
                : mInput(std::move(range))
            {}


            Iter begin() const
            {
                return Iter(mInput.begin());
            }


            Iter end() const
            {
                return Iter(mInput.begin());
            }


        private:
            Range mInput;
        };


        template <std::ranges::range Range>
        constexpr ChunkedRange<Range> operator()(Range range) const
        {
            return ChunkedRange(std::move(range));
        }

    private:
        unsigned mSize;
    };


    class Enumerate
        : public std::ranges::range_adaptor_closure<Enumerate>
    {
    public:
        template <std::ranges::range Range>
        constexpr auto operator()(Range range) const
        {
            return std::views::zip(std::views::iota(0), range);
        }
    };
}