/**
 –≠“È¿‡
*/

#ifndef __SUBPROTOCOL_H
#define __SUBPROTOCOL_H

#include <string>
#include "subsys.h"

using std::string;

#define SUBPROTOCOL_VER     1
#define SUBPROTOCOL_ID      "Protocol"
#define SUBPROTOCOL_NAME    "Transport Protocols"

class TransportOut;
class ModProtocol;

class ProtocolIn : public CtrlNode
{
public:
    ProtocolIn(const string &name);
    virtual ~ProtocolIn();

    const string &name()    {return mName;}
    const string &srcTr()   {return mSrcTr;}
    void setSrcTr(const string &vl) {mSrcTr = vl;}
    /* process input messages */
    virtual bool mess(const string &request,string &answer,const string &sender)
    {
        answer = "";return false;
    }

    ModProtocol &owner();
protected:

private:
    const string &nodeName() {return mName;}
    string mName;
    string mSrcTr;
};

class ModProtocol : public Module
{
public:
    ModProtocol(const string &id);
    virtual ~ModProtocol();

    /* input protocol */
    void list(vector<string> &list) {chldList(m_pr,list);}
    bool openStat(const string &name) {return chldPresent(m_pr,name);}
    void open(const string &name,const string &tr);
    void close(const string &name);

    AutoHD<ProtocolIn> at(const string &name) {return chldAt(m_pr,name);}

    /* output protocol */
    virtual void outMess(TransportOut &tro) {throw TError(nodePath().c_str(),"Function <%s> no support!","outMess");}
protected:
    virtual ProtocolIn *in_open(const string &name) {throw TError(nodePath().c_str(),"Function <%s> no support!","in_open");}
private:
    int m_pr;
};

class SubProtocol : public SubSys
{
public:
    SubProtocol();
    ~SubProtocol();

    int subVer()    {return SUBPROTOCOL_VER;}
    AutoHD<ModProtocol> at(const string &iid)   {return modAt(iid);}
protected:
    void load_();
    void save_();
private:

};


#endif // __SUBPROTOCOL_H
