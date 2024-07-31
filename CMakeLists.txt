cmake_minimum_required(VERSION 3.24)
project(eventplus)

set(CMAKE_CXX_STANDARD 17)

add_executable(eventplus
        src/main.cpp
        src/socket/TcpServerSocket.cpp
        src/socket/UdpServerSocket.cpp
        src/networkevent/SocketEventHandler.cpp
        src/common/SocketDetails.cpp
        src/socket/TcpClientSocket.cpp
        src/datatrasaction/NormalSocketHandler.cpp
        src/datatrasaction/TLSSocketHandler.cpp
        src/threadpool/ThreadPool.cpp
        src/threadpool/Task.cpp
        src/networkevent/EventScheduler.cpp
        src/networkevent/EventHandlerThread.cpp
        src/socket/SocketHandler.cpp
        src/common/SocketRemovalHandler.h
        src/workerthread/IOWorkerThreadHandler.cpp
        src/workerthread/IOWorkerThreadHandler.h
)
# Link with pthreads
find_package(Threads REQUIRED)
target_link_libraries(eventplus PRIVATE Threads::Threads)