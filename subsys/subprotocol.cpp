
#include "sys.h"
#include "subprotocol.h"


SubProtocol::SubProtcol() : SubSys(SUBPOTOCOL_ID,SUBPROTOCOL_NAME,true)
{

}

SubProtocol::~SubProtocol()
{

}

void SubProtocol::load_()
{

}

void SubProtocol::save_()
{

}


/**** mod suprotocol */
ModProtocol::ModProtocol(const string &id) : Module(id)
{
    m_pr = grpAdd("pr_");
}

ModProtocol::~ModProtocol()
{

}

void ModProtocol::open(const string &name,const string &tr)
{
    if(chldPresent(m_pr,name))
        return ;
    chldAdd(m_pr,in_open(name));
    at(name).at().setSrcTr(tr);
}

void ModProtocol::close(const string &name)
{
    chldDel(m_pr,name);
}


/***** protocol in */
ProtocolIn::ProtocolIn(const string &name):mName(name)
{

}

ProtocolIn::~ProtocolIn()
{

}

ModProtocol &ProtocolIn::owner()
{
    return *(ModProtocol*)nodePrev();
}
