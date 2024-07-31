//
// Created by Mohammad Ashraf Hossain on 7/25/2024.
//

#pragma once
#include "../common/CommonDefinition.h"
#include "../common/SocketDetails.h"

class SocketHandler {

public:
    int createServerSocket(std::string serverIP, std::string serverPort,SocketDetails& socketDetails);
    int createClientSocket(std::string serverIP,std::string strServerPort);
};



