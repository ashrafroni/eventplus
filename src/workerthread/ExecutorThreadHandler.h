//
// Created by Mohammad Ashraf Hossain on 8/2/2024.
//

#pragma once

#include "../threadpool/ThreadPool.h"

class ExecutorThreadHandler {
public:
    ExecutorThreadHandler(size_t numThreads);
    ~ExecutorThreadHandler();

private:
    ThreadPool m_threadPool;
};



