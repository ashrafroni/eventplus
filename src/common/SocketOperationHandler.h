//
// Created by Mohammad Ashraf Hossain on 7/28/2024.
//

#pragma once

#include "CommonDefinition.h"
#include "SocketDetails.h"


class SocketOperationHandler{
    virtual void removeSocket(EventStorePointer* eventStorePointer) = 0;
};