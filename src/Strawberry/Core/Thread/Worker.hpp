#pragma once
// Strawberry Core
#include "Strawberry/Core/Sync/Mutex.hpp"
// Standard Library
#include <atomic>
#include <deque>
#include <functional>
#include <thread>



namespace Strawberry::Core
{
	using Job = std::function<void()>;
	using JobQueue = Mutex<std::deque<Job>>;


	class Worker
	{
	public:
		Worker();
		Worker(const Worker&) = delete;
		Worker(Worker&&) = delete;
		Worker& operator=(const Worker&) = delete;
		Worker& operator=(Worker&&) = delete;


		~Worker();


		void Queue(Job&& job);
		void Join();
		void Run();


	private:
		Optional<Job> GetNextJob();


		std::atomic_bool mRunningFlag;
		std::thread mThread;
		JobQueue mJobQueue;
	};
}
