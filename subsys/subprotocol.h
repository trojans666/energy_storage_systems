/**
 –≠“È¿‡
*/

#ifndef __SUBPROTOCOL_H
#define __SUBPROTOCOL_H

class ModProtocol : public Module
{
public:

protected:

private:

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
