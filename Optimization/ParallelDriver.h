#pragma once
#include <thread>
#include <future>
#include <queue>
class ParallelDriver
{
	class FunctionThread
	{
	public:
		template<class Function>
		explicit FunctionThread(Function f): t(makeCatch(f))
		{}
		void join()
		{
			t.join();
		}
	private:
		std::thread t;

		template<class Function>
		auto makeCatch(Function f)->Function
		{
			return [f]()
			{
				try { f(); }
				catch (std::exception& e)
				{
					std::cout << e.what() << std::endl;
				}
			};
		}
	};

public:
	explicit ParallelDriver(unsigned int maxThreads = std::thread::hardware_concurrency()):maxThreads(maxThreads)
	{
		//std::cout << "driver has " << maxThreads << " threads." << std::endl;
	}

	template<class Function>
	void addTask(Function newTask)
	{
		tasks.emplace(newTask);
	}

	void run()
	{
		std::vector<FunctionThread> threads;
		while(!tasks.empty())
		{
			while(!tasks.empty() && threads.size()<maxThreads) // fill up to maxThreads or until no more tasks
			{
				threads.push_back(FunctionThread(std::move(tasks.front())));
				tasks.pop();
			}

			for(auto& thread:threads)
			{
				thread.join();
			}

			threads.clear();
		}
	}
private:
	unsigned int maxThreads;

	std::queue<std::function<void(void)>> tasks;
};