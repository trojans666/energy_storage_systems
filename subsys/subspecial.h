/**
用于特殊用途的类 比如热插拔 系统参数监听 工况检测等
*/

#ifndef __SUBSPECIAL_H
#define __SUBSPECIAL_H

#include <string>

#include "subsys.h"

using std::string;

#define SUBSPECIAL_VER      1
#define SUBSPECIAL_ID       "Special"
#define SUBSPECIAL_NAME     "Specials"

class ModSpecial : public Module
{
public:
    ModSpecial(const string &id);

protected:
    bool run_st; /* 运行标志 */
};


class SubSpecial : public SubSys
{
public:
    SubSpecial();
    int subVer()  {return SUBSPECIAL_VER;}

    AutoHD<ModSpecial> at(const string &iid) {return modAt(iid);}
protected:
    void load_() {}
    void save_() {}
private:

};


#endif // __SUBSPECIAL_H
