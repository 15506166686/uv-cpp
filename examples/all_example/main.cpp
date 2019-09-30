/*
    Copyright 2017, orcaer@yeah.net  All rights reserved.

    Author: orcaer@yeah.net

    Last modified: 2019-8-4

    Description: https://github.com/wlgq2/uv-cpp
*/


#include <iostream>


#include "EchoServer.h"
#include "Clinet.h"
#include <chrono>

using namespace uv;

#define      TEST_SIGNAL      1
#define      TEST_SERVER      1
#define      TEST_CLIENT      1
#define      TEST_ASYNC       1
#define      TEST_TIMER       1
#define      TEST_LOG         1
#define      TEST_UDP         1

#define       TEST_IPV6       0

int main(int argc, char** args)
{
    //�����¼��ַ�����
    EventLoop* loop = new EventLoop();
    //or
    //EventLoop* loop = EventLoop::DefalutLoop();

#if    TEST_SIGNAL
    //�ӹ�SIGPIPE�źš�
    Signal signal(loop,13,[](int sig)
    {
        //SIGPIPE
    });

#endif


    //server����
#if   TEST_SERVER
#if    TEST_IPV6
    SocketAddr addr1("0:0:0:0:0:0:0:0", 10002, SocketAddr::Ipv6);
#else
    SocketAddr addr1("0.0.0.0", 10002, SocketAddr::Ipv4);
#endif

    EchoServer server(loop, addr1);
    //������ʱ
    server.setTimeout(40);
    server.start();
#endif


    //client����
#if  TEST_CLIENT
#if    TEST_IPV6
    SocketAddr addr2("0:0:0:0:0:0:0:1", 10002, SocketAddr::Ipv6);
#else
    SocketAddr addr2("127.0.0.1", 10002, SocketAddr::Ipv4);
#endif
    Client client(loop);
    client.connectToServer(addr2);

#if  1

    //���̷߳�������
    std::thread thread([&client]()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        char* data = new char[4] {'t','e','s','t'};
       
        //�̰߳�ȫ;
        client.writeInLoop(data,sizeof(data),
            [](uv::WriteInfo& info)
        {
            //������Ҫ�ڷ�����ɻص����ͷ�
            //write message error.
            if (0 != info.status)
            {
                //��ӡ������Ϣ
                std::cout << "Write error ��" << EventLoop::GetErrorMessage(info.status) << std::endl;
            }
            delete[] info.buf;
        });
    });
#endif

#endif


    //loop�߳����첽ִ�к��������ڿ��̲߳�����
    //�����ԭ��libuv async�ӿڣ��޸��˵��ö�ο���ֻ����һ�ε����⡣
#if  TEST_ASYNC
    loop->runInThisLoop(
        []()
    {
        std::cout << "run function in loop thread one." << std::endl;
    });

    loop->runInThisLoop(
        []()
    {
        std::cout << "run function in loop thread two." << std::endl;
    });
#endif


    //��ʱ������
#if  TEST_TIMER
    Timer timer(loop, 1000, 1000,
        [](Timer*)
    {
        std::cout << "timer callback test..." << std::endl;
    });
    timer.start();

    //��ʱ��ֻ����һ�μ��ͷ�,������tcp����,������Ϣ��ʱ�ȡ�
    Timer* pTimer  =new Timer(loop, 1000, 0,
        [](Timer* handle)
    {
        std::cout << "timer callback run onice."<< std::endl;
        handle->close([](Timer* ptr)
        {
            std::cout << "release timer ptr."<< std::endl;
            //�ͷŶ�ʱ������
            delete ptr;
        });
    });
    pTimer->start();
#endif

    //log�ӿڰ�
#if  TEST_LOG
    auto log = uv::LogWriter::Instance();
    log->registerInterface(
        [](int level, const std::string& info)
    {
        std::cout << "log level " << level << " :" << info << std::endl;
    });
    log->setLevel(uv::LogWriter::Debug);
    log->debug("debug message");
    log->info("info message");
    log->warn("warn message");
    log->error("error message");
    log->fatal("fatal message");
#endif


#if  TEST_UDP
    SocketAddr addr3("127.0.0.1", 10003);
    uv::Udp udpReceive(loop, addr3);
    udpReceive.setMessageCallback(
        [&udpReceive](SocketAddr& from,const char* data,unsigned size)
    {
        std::string msg(data, size);
        std::cout << "udp receive message from "<< from.toStr()<<" :" << msg << std::endl;
        udpReceive.send(from, data, size);
    });
    udpReceive.startRead();

    SocketAddr addr4("127.0.0.1", 10004);
    uv::Udp udpSend(loop, addr4);
    udpSend.setMessageCallback(
        [](SocketAddr& from, const char* data, unsigned size)
    {
        std::string msg(data, size);
        std::cout << "udp call message :" << msg << std::endl;;
    });
    udpSend.startRead();
    char udpmsg[] = "udp test...";
    udpSend.send(addr3, udpmsg, sizeof(udpmsg));
#endif

    loop->run();
    delete loop;
}