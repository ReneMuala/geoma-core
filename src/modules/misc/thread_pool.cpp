//
// Created by dte on 10/23/2025.
//

#include "thread_pool.hpp"
#include <iostream>
namespace geoma::misc
{
    /**
     * @brief Spawns @p count workers that block on the task queue.
     *
     * Each worker loops: wait until a task is available or shutdown was
     * requested, then run the task. Workers exit only when @c stop is set
     * and the queue is drained, so no task is dropped.
     */
    thread_pool::thread_pool(size_t count)
    {
        std::cout << "\t[" << __FUNCTION__ << " starting with "<< count << " threads]" << std::endl;
        for (int i = 0; i < count; ++i)
        {
            threads.emplace_back([this]()
            {
                while (true)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mutex);

                        condition.wait(lock, [this]
                        {
                            return not tasks.empty() or stop;
                        });

                        if (stop and tasks.empty())
                        {
                            return;
                        }

                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    /**
     * @brief Sets the stop flag, wakes all workers and joins them.
     *
     * Because workers finish queued tasks before exiting, destruction
     * acts as a synchronization point for everything enqueued so far.
     */
    thread_pool::~thread_pool()
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            stop = true;
        }
        condition.notify_all();
        for (auto& thread : threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
    }

    /**
     * @brief Pushes the task and wakes one waiting worker.
     */
    void thread_pool::enqueue(std::function<void()> task)
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            tasks.emplace(std::move(task));
        }
        condition.notify_one();
    }
}
