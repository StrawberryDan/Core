#pragma once


#include "Strawberry/Core/Concepts.hpp"
#include "Strawberry/Core/Option.hpp"
#include "Strawberry/Core/Math/Periodic.hpp"


namespace Strawberry::Core::IO
{
	/// Class Definition
	template <typename Container, typename V> requires ReferenceIndexable<Container, V&> && (SizedContainer<Container> || std::same_as<std::vector<V>, Container>)
	class Circular
	{
	public:
		Circular(Container container)
			: mContainer(std::move(container))
			, mHead(Capacity(), 0)
			, mTail(Capacity(), 0)
			, mSize(0)
		{}


		Circular(Container container, size_t head, size_t tail)
			: mContainer(std::move(container))
			, mHead(Capacity(), head)
			, mTail(Capacity(), tail)
			, mSize(tail >= head ? tail - head : Capacity() - head + tail - 1)
		{}


		void Push(V value)
		{
			if constexpr (ResizableContainer<Container>)
			{
				Resize(Capacity() * 2);
			}

			mContainer[mTail++] = std::move(value);
			mSize = 1;
		}


		Option<V> Pop()
		{
			if (mSize == 0) return {};
			mSize -= 1;
			return std::move(mContainer[mHead++]);
		}


		size_t Size()       const { return mSize; }
		bool   Empty()      const { return Size() == 0; }
		size_t Capacity()   const requires SizedContainer<Container> { return mContainer.Size(); }
		size_t Capacity()   const requires std::same_as<std::vector<V>, Container> { return mContainer.size(); }
		bool   AtCapacity() const { Assert(Size() <= Capacity()); return Size() == Capacity(); }


	private:
		void Resize(size_t newSize) requires ResizableContainer<Container>
		{
			std::vector<V> values;
			values.reserve(Size());
			while (!Empty())
			{
				values.emplace_back(std::move(Pop().Unwrap()));
			}

			mContainer.Resize(newSize);
			mHead = 0;
			mTail = 0;
			mSize = 0;

			for (auto& value : values)
			{
				Push(std::move(value));
			}
		}


	private:
		Container                     mContainer;
		Math::DynamicPeriodic<size_t> mHead;
		Math::DynamicPeriodic<size_t> mTail;
		size_t                        mSize;
	};


	/// Type Deduction Guide
	template <typename Container> requires ReferenceIndexable<Container, decltype(std::declval<Container>()[std::declval<size_t>()])> && SizedContainer<Container>
	Circular(Container) -> Circular<Container, decltype(std::declval<Container>()[std::declval<size_t>()])>;
}
