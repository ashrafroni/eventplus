//
// Created by Ashraf Hossain on 7/24/2024.
//

#pragma once

#include "../common/CommonDefinition.h"
#include "../common/SocketDetails.h"

class SocketEventDispatcher
{
    virtual ~SocketEventDispatcher() = default;

    virtual void connectionEvent(std::unique_ptr<EventStorePointer> eventStorePointer) = 0;
    virtual void dataEvent(std::unique_ptr<EventStorePointer> eventStorePointer) = 0;
};
