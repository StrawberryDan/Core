#include <set>

#include "Strawberry/Core/Sync/ProducerConsumerQueue.hpp"
#include <thread>

#include "Strawberry/Core/Sync/Mutex.hpp"
#include "Strawberry/Core/Util/Ranges.hpp"
using namespace Strawberry;
using namespace Core;


void Test_LockFreeSWSRQueue()
{
	static constexpr size_t LIMIT = 1'000'000;


	LockFree::SPSCQueue<int, 256> queue;


	std::thread producer([&]()
	{
		for (int i = 0; i < LIMIT; i++)
		{
			while (!queue.Push(i));
		}
	});


	std::thread consume([&]()
	{
		for (int i = 0; i < LIMIT; i++)
		{
			Optional<int> x = queue.Pop();
			while (!x.HasValue()) x = queue.Pop();

			AssertEQ(x, i);
		}
	});

	producer.join();
	consume.join();
}


void Test_LockFreeMPMCQueue()
{
	static constexpr size_t LIMIT = 1'000'000;
	static constexpr size_t COUNT = 16;


	LockFree::MPMCQueue<int, LIMIT> queue;

	std::thread pushingThreads[COUNT];
	std::thread poppingThreads[COUNT];
	auto batches = std::views::iota(0) | std::views::take(LIMIT) | std::views::chunk(LIMIT / COUNT);

	Spinlock resultsLock;
	std::set<int> results;


	for (auto&& [i, batch] : Enumerate(batches))
	{
		pushingThreads[i] = std::thread([&, batch]
		{
			for (auto&& x : batch)
			{
				while (!queue.Push(x));
			}
		});
	}

	for (int i = 0; i < COUNT; i++)
	{
		pushingThreads[i].join();
	}


	for (auto& poppingThread : poppingThreads)
	{
		poppingThread = std::thread([&]()
		{
			while (true)
			{
				Optional<int> x = queue.Pop();

				bool finished = false;
				if (x.HasValue())
				{
					resultsLock.Lock();
					results.emplace(x.Value());
					finished = results.size() == LIMIT;
					resultsLock.Unlock();
				}
				else
				{
					resultsLock.Lock();
					finished = results.size() == LIMIT;
					resultsLock.Unlock();
				}

				if (finished) break;
			}
		});
	}


	for (int i = 0; i < COUNT; i++)
	{
		poppingThreads[i].join();
	}


	for (int i = 0; i < LIMIT; i++)
	{
		Assert(results.contains(i));
	}
}


int main()
{
	Test_LockFreeSWSRQueue();
	Test_LockFreeMPMCQueue();
}