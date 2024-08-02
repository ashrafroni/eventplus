//
// Created by Mohammad Ashraf Hossain on 8/2/2024.
//

#pragma once

#include "../common/CommonDefinition.h"
#include "../common/SocketDetails.h"

class DataEventDispatcher{
    virtual ~DataEventDispatcher() = default;
    virtual void dataEvent(std::unique_ptr<EventStorePointer> eventStorePointer, const std::string& data) = 0;
};
