cmake_minimum_required(VERSION 3.10)

# Set the project name
project(client)

# Specify the C++ standard (optional)
set(CMAKE_CXX_STANDARD 17)



# Add the executable
add_executable(client

        ../../src/socket/server/UdpServerSocket.cpp
        ../../src/networkevent/SocketEventHandler.cpp
        ../../src/common/SocketDetails.cpp
        ../../src/socket/client/TcpClientSocket.cpp
        ../../src/socket/server/TcpServerSocket.cpp
        ../../src/datatrasaction/server/TCPSocketHandler.cpp
        ../../src/datatrasaction/server/TLSSocketHandler.cpp
        ../../src/threadpool/ThreadPool.cpp
        ../../src/IOWorkerThread/IOTask.cpp
        ../../src/networkevent/EventScheduler.cpp
        ../../src/networkevent/EventHandlerThread.cpp
        ../../src/socket/SocketHandler.cpp
        ../../src/IOWorkerThread/IOWorkerThreadHandler.cpp
        ../../src/workerthread/ExecutorThreadHandler.cpp
        ../../src/ssl/SSLCertificateHandler.cpp
        ../../src/workerthread/OperationExecutorTask.cpp
        ../../src/socket/client/TLSClient.cpp
        ../../src/datatrasaction/client/TLSClientSocketHandler.cpp
        ../../src/datatrasaction/client/TCPClientSocketHandler.cpp
        ../../src/socket/client/TLSClient.cpp
        client.cpp)

# Optionally, link any required libraries for the client
# target_link_libraries(client_exe <library>)

find_package(Threads REQUIRED)
target_link_libraries(client PRIVATE Threads::Threads)

# Find and link with OpenSSL
find_package(OpenSSL REQUIRED)

target_link_libraries(client PRIVATE Threads::Threads OpenSSL::SSL OpenSSL::Crypto)
