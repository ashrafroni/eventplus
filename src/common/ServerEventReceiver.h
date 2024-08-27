//
// Created by Mohammad Ashraf Hossain on 8/2/2024.
//

#pragma once

#include "../common/CommonDefinition.h"
#include "../common/SocketDetails.h"

class ServerEventReceiver{
public:
    virtual ~ServerEventReceiver() = default;
    virtual void newConnectionEvent(EventStorePointer* eventStorePointer) = 0;
    virtual void connectionClosedEvent(EventStorePointer* eventStorePointer) = 0;
    virtual void dataEvent(EventStorePointer* eventStorePointer) = 0;
};
