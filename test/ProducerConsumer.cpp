#include "Strawberry/Core/Sync/ProducerConsumerQueue.hpp"
#include <thread>
using namespace Strawberry;
using namespace Core;


int main()
{
	LockFreeSWSRQueue<int, 4> queue;


	std::thread producer([&]()
	{
		for (int i = 0; i < 1'000'000; i++)
		{
			while (!queue.Push(i));
		}
	});


	std::thread consume([&]()
	{
		for (int i = 0; i < 1'000'000; i++)
		{
			Optional<int> x = queue.Pop();
			while (!x.HasValue()) x = queue.Pop();

			AssertEQ(x, i);
		}
	});

	producer.join();
	consume.join();
}