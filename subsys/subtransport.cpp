
#include "sys.h"
#include "module.h"
#include "subprotocol.h"
#include "subtransport.h"


/******* sub transport ******/
SubTransport::SubTransport() :SubSys(SUBTRANSPORT_ID,SUBTRANSPORT_NAME,true)
{

}

SubTransport::~SubTransport()
{

}

void SubTransport::inTrList(vector<string> &ls)
{
    ls.clear();

    vector<string> t_ls,m_ls;
    modList(t_ls);
    for(unsigned int i_tp = 0; i_tp < t_ls.size(); i_tp++)
    {
        at(t_ls[i_tp]).at().inList(m_ls);
        for(unsigned int i_t = 0; i_t < m_ls.size(); i_t++)
        {
            ls.push_back(t_ls[i_tp]+"."+m_ls[i_t]);
        }
    }
}

void SubTransport::outTrList(vector<string> &ls)
{
    ls.clear();

    vector<string> t_ls,m_ls;
    modList(t_ls);

    for(unsigned int i_tp = 0; i_tp < t_ls.size(); i_tp++)
    {
        at(t_ls[i_tp]).at().outList(m_ls);
        for(unsigned int i_t = 0; i_t < m_ls.size(); i_t++)
        {
            ls.push_back(t_ls[i_tp]+"."+m_ls[i_t]);
        }
    }
}

void SubTransport::load_()
{

}

void SubTransport::save_()
{

}

void SubTransport::subStart()
{
    vector<string> t_lst, o_lst;
    modList(t_lst);
    for( unsigned int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
        AutoHD<ModTransport> mod = modAt(t_lst[i_t]);
        o_lst.clear();
        mod.at().inList(o_lst);
        for( unsigned int i_o = 0; i_o < o_lst.size(); i_o++ )
            try
            {
                AutoHD<TransportIn> in = mod.at().inAt(o_lst[i_o]);
                if( !in.at().startStat() && in.at().toStart() )
                    in.at().start();
            }
            catch( TError err )
            {

            }

        o_lst.clear();
        mod.at().outList(o_lst);
        for( unsigned int i_o = 0; i_o < o_lst.size(); i_o++ )
            try
            {
                AutoHD<TransportOut> out = mod.at().outAt(o_lst[i_o]);
                if( !out.at().startStat() && out.at().toStart() )
                    out.at().start();
            }
            catch( TError err )
            {

            }
    }

    //> Controllers start
    SubSys::subStart( );
}

void SubTransport::subStop()
{
    vector<string> t_lst, o_lst;
    modList(t_lst);
    for( unsigned int i_t = 0; i_t < t_lst.size(); i_t++ )
    {
        AutoHD<ModTransport> mod = modAt(t_lst[i_t]);
        o_lst.clear();
        mod.at().inList(o_lst);
        for( unsigned int i_o = 0; i_o < o_lst.size(); i_o++ )
            try
            {
                AutoHD<TransportIn> in = mod.at().inAt(o_lst[i_o]);
                if( in.at().startStat() )
                    in.at().stop();
            }
            catch( TError err )
            {

            }
        o_lst.clear();
        mod.at().outList(o_lst);
        for( unsigned int i_o = 0; i_o < o_lst.size(); i_o++ )
            try
            {
                AutoHD<TransportOut> out = mod.at().outAt(o_lst[i_o]);
                if( out.at().startStat() )
                    out.at().stop();
            }
            catch( TError err )
            {

            }
    }

    SubSys::subStop( );
}


void SubTransport::extHostList( const string &user, vector<string> &list )
{
    list.clear();
    ResAlloc res(extHostRes,false);
    for( unsigned int i_h = 0; i_h < extHostLs.size(); i_h++ )
        if( !user.size() || user == extHostLs[i_h].user_open )
            list.push_back(extHostLs[i_h].id);
}

bool SubTransport::extHostPresent( const string &user, const string &iid )
{
    ResAlloc res(extHostRes,false);
    for( unsigned int i_h = 0; i_h < extHostLs.size(); i_h++ )
        if( (!user.size() || user == extHostLs[i_h].user_open) && extHostLs[i_h].id == iid )
            return true;
    return false;
}

void SubTransport::extHostSet( const ExtHost &host )
{
    ResAlloc res(extHostRes,true);
    for( unsigned int i_h = 0; i_h < extHostLs.size(); i_h++ )
        if( host.user_open == extHostLs[i_h].user_open && extHostLs[i_h].id == host.id )
        { extHostLs[i_h] = host; return; }
    extHostLs.push_back(host);
}

void SubTransport::extHostDel( const string &user, const string &id )
{
    ResAlloc res(extHostRes,true);
    for( unsigned int i_h = 0; i_h < extHostLs.size(); )
	if( (!user.size() || user == extHostLs[i_h].user_open) && extHostLs[i_h].id == id )
	    extHostLs.erase(extHostLs.begin()+i_h);
	else i_h++;
}

SubTransport::ExtHost SubTransport::extHostGet( const string &user, const string &id )
{
    ResAlloc res(extHostRes,false);
    for( unsigned int i_h = 0; i_h < extHostLs.size(); i_h++ )
	if( (user.empty() || user == extHostLs[i_h].user_open) && extHostLs[i_h].id == id )
	    return extHostLs[i_h];
    return ExtHost(user,"","","","","","");
}

AutoHD<TransportOut> SubTransport::extHost( SubTransport::ExtHost host, const string &pref )
{
    if( !host.id.size() || !modPresent(host.transp) )
	throw TError(nodePath().c_str(),"Remote host error!");

    if( !at(host.transp).at().outPresent(pref+host.id) )
	at(host.transp).at().outAdd(pref+host.id);
    if( at(host.transp).at().outAt(pref+host.id).at().addr() != host.addr )
    {
	at(host.transp).at().outAt(pref+host.id).at().setAddr(host.addr);
	at(host.transp).at().outAt(pref+host.id).at().stop();
    }

    return at(host.transp).at().outAt(pref+host.id);
}

/******** mod transport *********/
ModTransport::ModTransport(const string &id) : Module(id)
{
    mIn = grpAdd("in_");
    mOut = grpAdd("out_");
}

ModTransport::~ModTransport()
{
    nodeDelAll();
}


SubTransport &ModTransport::owner( )	{ return (SubTransport&)Module::owner(); }

void ModTransport::inAdd( const string &name)
{
    if( chldPresent(mIn,name) ) return;
    chldAdd(mIn,In(name));
}

void ModTransport::outAdd( const string &name)
{
    if( chldPresent(mOut,name) ) return;
    chldAdd(mOut,Out(name));
}



//************************************************
//* TTransportIn				 *
//************************************************
TransportIn::TransportIn( const string &iid) :
    run_st(false), mId(iid), mName(""),
    mDscr(""), mAddr(""), mProt(""),
    mStart(true)
{
    mId = iid;
}

TransportIn::~TransportIn()
{

}


ModTransport &TransportIn::owner( )	{ return *(ModTransport*)nodePrev(); }

string TransportIn::name( )		{ return mName.size()?mName:mId; }

string TransportIn::protocol( )	{ return SYS::strParse(protocolFull(),0,"."); }


string TransportIn::getStatus( )
{
    return startStat() ? "Started. " : "Stoped. ";
}

void TransportIn::load_( )
{

}

void TransportIn::save_( )
{

}


//************************************************
//* TTransportOut                                *
//************************************************
TransportOut::TransportOut( const string &iid) :
    run_st(false), mId(""), mName(""),
    mDscr(""), mAddr(""), mStart(true)
{
    mId = iid;
}

TransportOut::~TransportOut( )
{

}


ModTransport &TransportOut::owner( )	{ return *(ModTransport*)nodePrev(); }

string TransportOut::name()		{ return mName.size()?mName:mId; }


string TransportOut::getStatus( )
{
    return startStat() ? "Started. " : "Stoped. ";
}

void TransportOut::load_( )
{

}

void TransportOut::save_()
{

}
