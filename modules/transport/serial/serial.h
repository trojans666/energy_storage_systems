#ifndef __SERIAL_H
#define __SERIAL_H

#include "event.h"
#include "subtransport.h"

/**
该类表示所有的serial,而不是一个物理口一个类
*/
class TransSerial : public ModTransport
{
public:
    TransSerial(string name);
    ~TransSerial();

    /** 内部创建一个线程 监听所有的serial fd  */
    void modStart();
    void modStop();


protected:

private:

};

extern TransSerial *mod;

#endif // __SERIAL_H
