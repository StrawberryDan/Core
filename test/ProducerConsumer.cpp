#include "Strawberry/Core/Sync/ProducerConsumerQueue.hpp"
#include <thread>
using namespace Strawberry;
using namespace Core;


void Test_LockFreeSWSRQueue()
{
	static constexpr size_t LIMIT = 1'000'000;

	LockFreeSWSRQueue<int, 256> queue;


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


int main()
{
	Test_LockFreeSWSRQueue();
}