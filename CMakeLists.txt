cmake_minimum_required(VERSION 3.24)
project(eventplus)

set(CMAKE_CXX_STANDARD 17)

add_executable(eventplus
        src/main.cpp
        src/socket/server/UdpServerSocket.cpp
        src/networkevent/SocketEventHandler.cpp
        src/common/SocketDetails.cpp
        src/socket/client/TcpClientSocket.cpp
        src/socket/server/TcpServerSocket.cpp
        src/datatrasaction/server/TCPSocketHandler.cpp
        src/datatrasaction/server/TLSSocketHandler.cpp
        src/threadpool/ThreadPool.cpp
        src/IOWorkerThread/IOTask.cpp
        src/networkevent/EventScheduler.cpp
        src/networkevent/EventHandlerThread.cpp
        src/socket/SocketHandler.cpp
        src/common/SocketRemovalHandler.h
        src/IOWorkerThread/IOWorkerThreadHandler.cpp
        src/IOWorkerThread/IOWorkerThreadHandler.h
        src/workerthread/ExecutorThreadHandler.cpp
        src/workerthread/ExecutorThreadHandler.h
        src/common/EventReceiver.h
        src/ssl/SSLCertificateHandler.cpp
        src/ssl/SSLCertificateHandler.h
        src/workerthread/OperationExecutorTask.cpp
        src/workerthread/OperationExecutorTask.h
        src/socket/client/TLSClient.cpp
        src/socket/client/TLSClient.h
        src/datatrasaction/client/TLSClientSocketHandler.cpp
        src/datatrasaction/client/TLSClientSocketHandler.h
        src/datatrasaction/client/TCPClientSocketHandler.cpp
        src/datatrasaction/client/TCPClientSocketHandler.h
)
# Link with pthreads
find_package(Threads REQUIRED)
target_link_libraries(eventplus PRIVATE Threads::Threads)

# Find and link with OpenSSL
find_package(OpenSSL REQUIRED)

target_link_libraries(eventplus PRIVATE Threads::Threads OpenSSL::SSL OpenSSL::Crypto)