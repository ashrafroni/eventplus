//
// Created by Mohammad Ashraf Hossain on 7/25/2024.
//

#include "EventHandlerThread.h"


void EventHandlerThread::startReceivingEvent(){
//    setupSignalHandler();
    startPolling();
}

void EventHandlerThread::startEventReceiverThread(){
    m_eventHandlerThread = std::make_unique<std::thread>(&EventHandlerThread::startReceivingEvent, this);
}


void EventHandlerThread::stopEventThread(){
    std::cout << "EventHandlerThread::stopEventThread()" << std::endl;
    stopPolling();
//    pthread_kill(m_eventHandlerThread->native_handle(), SIGUSR1);
//    if (m_eventHandlerThread && m_eventHandlerThread->joinable()) {
//        m_eventHandlerThread->detach();
//    }
    closeEpoll();
}

EventHandlerThread::EventHandlerThread(){
}


EventHandlerThread::~EventHandlerThread(){
    stopPolling();
    closeEpoll();
}

//void signal_handler(int signum) {
//
//}
//void EventHandlerThread::setupSignalHandler(){
//    struct sigaction sa;
//    sa.sa_handler = signal_handler;
//    sa.sa_flags = 0;
//    sigemptyset(&sa.sa_mask);
//
//    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
//        perror("sigaction");
//        exit(EXIT_FAILURE);
//    }
//}