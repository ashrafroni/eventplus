//
// Created by Dell on 8/27/2024.
//

#pragma once


#include "../common/CommonDefinition.h"
#include "../common/SocketDetails.h"



class ClientEventReceiver{
public:
    virtual void dataEvent(EventStorePointer* eventStorePointer) = 0;
};
