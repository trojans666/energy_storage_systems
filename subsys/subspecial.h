/**
����������;���� �����Ȳ�� ϵͳ�������� ��������
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
    bool run_st; /* ���б�־ */
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
