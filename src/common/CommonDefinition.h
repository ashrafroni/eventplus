//
// Created by Ashraf on 7/6/2024.
//

#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <mutex>
#include <thread>
#include <queue>


enum SocketType
{
    SocketType_NO = 0,
    SocketType_UDP,
    SocketType_TCP,
    SocketType_TLS
};
enum SocketStatus
{
    SocketStatus_New = 0,
    SocketStatus_Ready_for_operation,
    SocketType_Data_incoming,
    SocketType_Data_read_Completed
};