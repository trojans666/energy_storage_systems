/**
 归档类
*/

#ifndef __SUBARCHIVE_H
#define __SUBARCHIVE_H


#define SUBARCHIVE_VER      1
#define SUBARCHIVE_ID
#define SUBARCHIVE_NAME


class SubArchive;

class MessArchivator : public CtrlNode
{
public:

protected:

private:

};

/* mod archive */
class ModArchive : public Module
{
public:
    ModArchive(const string &id);
    virtual ~ModArchive();

    /* message */
    void messList(vector<string> &list) {chldList(mMess,list);}
    bool messPresent(const string &iid) {return chldPresent(mMess,iid);}
    void messAdd(const string &iid);
    void messDel(const string &iid,bool full = false) {return chldDel(mMess,iid);}

    AutoHD<MessArchivator> messAt(const string &iid) {return chldAt(mMess,iid);}

    SubArchive &owner();

protected:

private:
    int mMess;
};

/* sub archive */
class SubArchive : public SubSys
{
public:
    SubArchive();
    ~SubArchive();

    int subVer()        {return SUBARCHIVE_VER;}

    void subStart();
    void subStop();

    /* 包含的所有module  */
    AutoHD<ModArchive> at(const string &name) {return modAt(name);}

    /* message function */
    void messPut(const char *data);
    void messPut(time_t tm,const string &cat,char level,const string &mess);

protected:
    void load_() {}
    void save_() {}
private:

};

#endif // __SUBARCHIVE_H
