#pragma once
#include <concepts>
#include <cstdint>
#include <map>
#include "Strawberry/Core/Types/Optional.hpp"

namespace Strawberry::Core
{
	struct AllocatorSegment
	{
		size_t position;
		size_t extent;
	};


	struct AllocationRequest
	{
		size_t count;
		size_t alignment = 1;

		AllocationRequest& WithCount(size_t _count)
		{
			this->count = _count;
			return *this;
		}


		AllocationRequest& WithAlignment(size_t _alignment)
		{
			this->alignment = _alignment;
			return *this;
		}
	};


	template <typename Config>
	concept AllocatorConfig = requires
	{
		typename Config::ResourceType;
		typename Config::SegmentType;

		{ std::invoke(Config::AllocateFN, std::declval<const typename Config::Resource&>(), std::declval<const AllocatorConfig&>()) } -> std::convertible_to<Optional<typename Config::SegmentType>>;
		{ std::invoke(Config::DeallocateFN, std::declval<const typename Config::ResourceType&>(), std::declval<const typename Config::SegmentType&>()) };
		{ std::invoke(Config::ResourceSizeFN, std::declval<const typename Config::ResourceType>()) } -> std::convertible_to<size_t>;
	};


	template <AllocatorConfig _Config>
	class FreelistAllocator
	{
	public:
		using Config = _Config;


		FreelistAllocator(Config::Resource&& resource)
			: mResource(std::move(resource))
		{
			mFreeLists.emplace(0, Config::ResourceSizeFN(mResource));
		}


		Optional<typename Config::SegmentType> Allocate(const AllocationRequest& request)
		{
			for (const auto& [pos, size] : mFreeLists)
			{
				auto aligned = NextAlignedAddress(pos, request.alignment);
			}
		}


		void Free(Config::SegmentType&& segment)
		{
			Config::DeallocateFN(mResource, std::move(segment));
		}


	private:
		static size_t NextAlignedAddress(size_t address, size_t alignment)
		{
			auto mod = address % alignment;
			auto offset = mod == 0 ? 0 : alignment - mod;
			return address + offset;
		}

		Config::ResourceType mResource;

		// Mapping of positions to lengths, representing free segments in the resource;
		std::map<size_t, size_t> mFreeLists;
	};


	template <AllocatorConfig _Config>
	class BuddyAllocator
	{
	public:
		using Config = _Config;


		Config::SegmentType Allocate(const AllocationRequest& request)
		{

		}


		void Free(Config::SegmentType&& segment)
		{

		}

	private:
		Config::ResourceType mResource;
	};
}
