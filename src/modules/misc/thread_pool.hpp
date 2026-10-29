//
// Created by dte on 10/23/2025.
//

#pragma once
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>
#include <vector>

/**
 * @namespace geoma::misc
 * @brief Small utilities shared across modules.
 */
namespace geoma::misc
{
    /**
     * @brief A minimal fixed-size worker thread pool.
     *
     * Workers spin on a task queue protected by a mutex/condition pair.
     * Tasks are plain @c std::function<void()> and run in unspecified
     * threads; synchronize shared data yourself (importers use a mutex for
     * that). Destroying the pool signals shutdown and joins every worker,
     * so all queued tasks are guaranteed to have run.
     */
    class thread_pool {
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks;
        std::mutex mutex;
        std::condition_variable condition;
        bool stop = false;
    public:
        /**
         * @brief Creates a pool and starts its workers.
         * @param count Number of worker threads; defaults to the hardware
         *              concurrency reported by the platform.
         */
        explicit thread_pool(size_t count = std::thread::hardware_concurrency());

        /** @brief Signals shutdown and joins all workers. */
        ~thread_pool();

        /**
         * @brief Schedules a task for execution by one of the workers.
         * @param task Callable to run; executed in arbitrary order.
         */
        void enqueue(std::function<void()> task);
    };
}

