/**
 设备接口类 如 serial can network
*/

#ifndef __SUBTRANSPORT_H
#define __SUBTRANSPORT_H

#include <string>
#include <vector>

#include "resalloc.h"

using std::vector;
using std::string;

#define SUBTRANSPORT_VER    1
#define SUBTRANSPORT_ID     "Transport"
#define SUBTRANSPORT_NAME   "Transports"

class ModTransport;

class TransportIn : public CtrlNode
{
public:
    TransportIn(const string &id);
    virtual ~TransportIn();

    const string &id() {return mId;}
    string name();
    string dscr()   {return mDscr;}
    string addr()   {return mAddr;}
    string protocolFull()   {return mProt;}
    string protocol();
    virtual string getStatus();

    bool toStart()  {return mStart;}
    bool startStat()    {return run_st;}

    void setName(const string &inm) {mName = inm;}
    void setDscr(const string &idscr)   {mDscr = idscr;}
    virtual void setAddr(const string &addr)    {mAddr = addr;}
    void setProtocolFull(const string &prt) {mProt = prt;}
    void setToStart(bool val)   {mStart = val;}

    virtual void start() {}
    virtual void stop() {}

    ModTransport &owner();

protected:
    void load_();
    void save_();
    bool run_st;
private:
    const string &nodeName() {return mId;}

    string mId;
    string mName;
    string mDscr;
    string mAddr;
    string mProt;
    string mStart;
};

/******** transport out **************/
class TransportOut : public CtrlNode
{
public:
    TransportOut(const string &id);
    virtual ~TransportOut();

    const string &id() {return mId;}
    string name();
    string dscr()   {return mDscr;}
    string addr()   {return mAddr;}

    bool toStart()  {return mStart;}
    bool startStat()    {return run_st;}
    virtual string getStatus();

    void setName(const string &inm) {mName = inm;}
    void setDscr(const string &idscr) {mDscr = idscr;}
    virtual void setAddr(const string &addr) {mAddr = addr;}
    void setToStart(bool val) {mStart = val;}

    virtual void start() {}
    virtual void stop() {}

    virtual int MessIO(const char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0, int time = 0, bool noRes = false )
    {
        return 0;
    }

    ModTransport &owner();
    ResRW &nodeRes()    {return nRes;}

protected:
    void load_();
    void save_();

    bool run_st;
private:
    const string &nodeName() {return mId;}

    string mId;
    string mName;
    string mDscr;
    string mAddr;
    string mStart;

    ResRW nRes;
};

class ModTransport : public Module
{
public:
    ModTransport(const string &id);
    virtual ~ModTransport();

    /* input transport */
    void inList(vector<string> &list) {chldList(mIn,list);}
    bool inPresent(const string &name) {return chldPresent(mIn,name);}
    void inAdd(const string &name);
    void inDel(const string &name,bool complete = false) {chldDel(mIn,name,-1,complete); }
    AutoHD<TransportIn> inAt(const string &name) {return chldAt(mIn,name);}

    /* output transport */
    void outList(vector<string> &list)  {chldList(mOut,list);}
    bool outPresent(const string &name) {return chldPresent(mOut,name);}
    void outAdd(const string &name);
    void outDel(const string &name,bool complete = false) { chldDel(mOut,name,-1,complete);}
    AutoHD<TransportOut> outAt(const string &name) {return chldAt(mOut,name);}

    SubTransport &owner();

protected:
    virtual TransportIn *In(const string &name)
    {
        throw TError(nodePath().c_str(),"Input transport no support!");
    }
    virtual TransportOut *Out(const string &name)
    {
        throw TError(nodePath().c_str(),"Output transport no support!");
    }
private:
    int mIn,mOut;
};

class SubTransport : public SubSys
{
public:
    class ExtHost
	{
	    public:
		//Methods
		ExtHost( const string &iuser_open, const string &iid, const string &iname,
			    const string &itransp, const string &iaddr, const string &iuser, const string &ipass ) :
		    user_open(iuser_open), id(iid), name(iname), transp(itransp), addr(iaddr),
		    user(iuser), pass(ipass), link_ok(false) { }

		//Attributes
		string	user_open;	//User which open remote host
		string	id;		//External host id
		string	name;		//Name
		string	transp;		//Connect transport
		string	addr;		//External host address
		string	user;		//External host user
		string	pass;		//External host password
		bool	link_ok;	//Link OK
	};

	SubTransport();
	~SubTransport();

	int subVer()    {return SUBTRANSPORT_VER;}
	void inTrList(vector<string> &ls);
	void outTrList(vector<string> &ls);

	void extHostList(const string &user,vector<string> &list);
	bool extHostPresent(const string &user,const string &id);

	AutoHD<TransportOut> extHost(Transport::ExtHost host,const string &pref = "" );
	ExtHost extHostGet(const string &user,const string &id);
	void extHostSet(const ExtHost &host);
	void extHostDel(const string &user,const string &id);

	void subStart();
	void subStop();

	AutoHD<ModTransport> at(const string &iid) {return modAt(iid);}
protected:
    void load_();
    void save_();
private:
    ResRW	extHostRes;             //External hosts resource
	vector<ExtHost> extHostLs;      //External hosts list
};


#endif // __SUBTRANSPORT_H
