#ifndef __SERIAL_H
#define __SERIAL_H

#include "event.h"
#include "subtransport.h"

/**
�����ʾ���е�serial,������һ�������һ����
*/
class TransSerial : public ModTransport
{
public:
    TransSerial(string name);
    ~TransSerial();

    /** �ڲ�����һ���߳� �������е�serial fd  */
    void modStart();
    void modStop();


protected:

private:

};

extern TransSerial *mod;

#endif // __SERIAL_H
