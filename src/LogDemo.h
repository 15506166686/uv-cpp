#ifndef LOG_DEMO_H
#define LOG_DEMO_H

#include <uv/LogInterface.h>
#include <iostream>

//log interface instructions, can be bound to your log libary.
//log�ӿ��÷����ɰ󶨵�ʵ��log�⡣
class  LogDemo : public uv::LogInterface
{
public:
    LogDemo()
    {
        
    }
    void warn(std::string& data) override
    {
        std::cout << "warn :" << data << std::endl;
    }
    virtual void error(std::string& data) override
    {
        std::cout << "error :" << data << std::endl;
    }
    virtual void info(std::string& data) override
    {
        std::cout << "info :" << data << std::endl;
    }
    virtual void debug(std::string& data) override
    {
        std::cout << "debug :" << data << std::endl;
    }

};

#endif
