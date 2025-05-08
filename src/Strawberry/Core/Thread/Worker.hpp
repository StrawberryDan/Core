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
	using Task = std::function<void()>;
	using TaskQueue = Mutex<std::deque<Task>>;


	class Worker
	{
	public:
		Worker();
		Worker(const Worker&) = delete;
		Worker(Worker&&) = delete;
		Worker& operator=(const Worker&) = delete;
		Worker& operator=(Worker&&) = delete;


		~Worker();


		void Queue(Task&& job);
		void Join();
		void Run();


	private:
		Optional<Task> GetNextJob();


		std::atomic_bool mRunningFlag;
		std::thread mThread;
		TaskQueue mJobQueue;
	};
}
