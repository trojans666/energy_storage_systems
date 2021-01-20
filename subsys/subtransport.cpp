
#include "sys.h"
#include "stropt.h"
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


void SubTransport::load_()
{

}

void SubTransport::save_()
{

}

void SubTransport::subStart()
{
    #if 0
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
#endif
    //> Controllers start
    SubSys::subStart( );
}

void SubTransport::subStop()
{
    #if 0
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
#endif
    SubSys::subStop( );
}


/******** mod transport *********/
ModTransport::ModTransport(const string &id) : Module(id)
{

}

ModTransport::~ModTransport()
{
    nodeDelAll();
}


SubTransport &ModTransport::owner( )	{ return (SubTransport&)Module::owner(); }




