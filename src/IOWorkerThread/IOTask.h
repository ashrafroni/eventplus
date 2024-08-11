//
// Created by Mohammad Ashraf Hossain on 7/24/2024.
//

#pragma once

#include <functional>
#include "../common/CommonDefinition.h"
#include "../common/SocketDetails.h"

class IOTask {
public:
    IOTask(){
        m_taskExecutionFunc = NULL;
        m_eventStorePointer = NULL;
    }

    IOTask(EventStorePointer* eventStorePointer, std::function<void(EventStorePointer*)> taskExecutionFuncParam
         )
            : m_taskExecutionFunc(taskExecutionFuncParam),
              m_eventStorePointer(std::move(eventStorePointer)) {
    }

    void execute() {
        if (m_taskExecutionFunc) {
            m_taskExecutionFunc(m_eventStorePointer);
        }
    }

private:
    std::function<void(EventStorePointer*)> m_taskExecutionFunc;
    EventStorePointer* m_eventStorePointer;
};



