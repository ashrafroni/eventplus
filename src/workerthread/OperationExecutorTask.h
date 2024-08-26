//
// Created by Mohammad Ashraf Hossain on 8/11/2024.
//

#pragma once

#include <functional>
#include "../common/CommonDefinition.h"
#include "../common/SocketDetails.h"

class OperationExecutorTask {


private:
    std::function<void(EventStorePointer*)> m_taskExecutionFunc;
    EventStorePointer* m_eventStorePointer;
};



