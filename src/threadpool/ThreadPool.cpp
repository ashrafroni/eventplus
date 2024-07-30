//
// Created by Ashraf Hossain on 7/24/2024.
//

#include "ThreadPool.h"


ThreadPool::ThreadPool(size_t numThreads) : m_stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        m_workers.emplace_back(&ThreadPool::workerThread, this);
    }
}

ThreadPool::~ThreadPool() {
    m_stop.store(true);
    m_condition.notify_all();
    for (std::thread &worker : m_workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::enqueueTask(std::function<void(EventStorePointer*)> taskFunction) {
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_tasks.emplace();
    }
    m_condition.notify_one();
}

void ThreadPool::workerThread() {
    while (!m_stop.load()) {

        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_condition.wait(lock, [this] { return m_stop.load() || !m_tasks.empty(); });
        if (m_stop.load() && m_tasks.empty()) {
            return;
        }
        Task task = m_tasks.front();
        m_tasks.pop();
        task.execute();
    }
}