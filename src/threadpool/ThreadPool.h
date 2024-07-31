//
// Created by Ashraf Hossain on 7/24/2024.
//

#pragma once


#include <functional>
#include <condition_variable>
#include <atomic>
#include "../common/CommonDefinition.h"
#include "Task.h"


class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    void enqueueTask(EventStorePointer* eventStorePointer, std::function<void(EventStorePointer*)> taskFunction);

private:
    std::vector<std::thread> m_workers;
    std::queue<Task> m_tasks;

    std::mutex m_queueMutex;
    std::condition_variable m_condition;
    std::atomic<bool> m_stop;

    void workerThread();
};


