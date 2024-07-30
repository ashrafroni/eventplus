//
// Created by Ashraf on 7/6/2024.
//

#pragma once

#include "CommonDefinition.h"
#include "SocketDetails.h"

class EventDispatcher {
public:
    virtual ~EventDispatcher() = default;

    virtual void handleConnectionEvent(EventStorePointer* eventStorePointer) = 0;
};

