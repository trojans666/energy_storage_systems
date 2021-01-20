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

class SubTransport;

class ModTransport : public Module
{
public:
    ModTransport(const string &id);
    virtual ~ModTransport();


    SubTransport &owner();

protected:

private:

};

/** 仅仅作为 接口 */
class SubTransport : public SubSys
{
public:
	SubTransport();
	~SubTransport();

	int subVer()    {return SUBTRANSPORT_VER;}
    virtual void perCall(unsigned int timeout);
	void subStart();
	void subStop();
    /** 每个transport通过 id标识自己 */
	AutoHD<ModTransport> at(const string &iid) {return modAt(iid);}
protected:
    void load_();
    void save_();
private:

};


#endif // __SUBTRANSPORT_H
