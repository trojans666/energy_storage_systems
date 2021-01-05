/**
  子系统类
*/

#ifndef __SUBSYS_H
#define __SUBSYS_H

#include <string>
#include <vector>

#include "module.h"

using std::string;
using std::vector;

class ModSchedul;
class SYS;

class SubSys : public CtrlNode
{
public:
    SubSys(const char *id,const char *name,bool modi = false); /** modi 是否创建module group */
    virtual ~SubSys();

    const string &subId() {return mId;}
    string subName();
    virtual int subVer() {return 0;} /*子系统版本 */

    bool subModule() {return mSubModule;} /*是否有module */

    virtual void subStart();
    virtual void subStop();

    /* modules */
    void modList(vector<string> &list);
    bool modPresent(const string &name);
    void modAdd(Module *mod);
    void modDel(const string &name);
    AutoHD<Module> modAt(const string &name);

    SYS &owner();
    /** 是否已经启动 substart */
    bool subStartStat() {return mStart;}
protected:
    bool mStart;
private:
    /* 节点名称 */
    const string &nodeName() {return subId();}
    bool mSubModule; /*是否有module */
    int mMod; /*grp id */
    string mId;
    string mName;
};

#endif
