#ifndef __SERIAL_H
#define __SERIAL_H

#include "subtransport.h"

class SerialIn : public TransportIn
{
public:
    SerialIn(string name);
    ~SerialIn();
protected:

private:

};

class SerialOut : public TransportOut
{
public:
    SerialOut(string name);
    ~SerialOut();
protected:

private:

};

class TransSerial : public ModTransport
{
public:
    TransSerial(string name);
    ~TransSerial();

    void modStart();
    void modStop();

    AutoHD<SerialIn> inAt(const string &name);
    AutoHD<SerialOut> outAt(const string &name);

    SerialIn *In(const string &name);
    SerialOut *Out(const string &name);

    static void writeLine(int fd,const string &ln);
    static string expect(int fd,const string &expLst,int tm);

protected:

private:

};

extern TransSerial *mod;

#endif // __SERIAL_H
