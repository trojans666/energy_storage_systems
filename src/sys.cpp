#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdarg.h>

#ifdef __linux__
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#endif // __linux__

#include "sys.h"

SYS *sys;

SYS::SYS(int argi, char **argb, char **env)
    :mArgc(argi)
    ,mArgv((const char **)argb)
    ,mEnvp((const char **)env)
    ,mUser("root")
    ,mCfgFile("cfg/config.xml")
    ,mId("Scada_Id")
    ,mName("Scada_Name")
    ,mIconDir("pic/")
    ,mModDir("modules/")
    ,mStopFlg(-1)
{
    sys = this;
    mSubSt = grpAdd("sub_",true); /*true ʱ������ */

    /** set signal */
    signal(SIGINT,sighandler);
    signal(SIGTERM,sighandler);
    signal(SIGALRM,sighandler);
    signal(SIGPIPE,sighandler);
    signal(SIGABRT,sighandler);
    signal(SIGCHLD,sighandler);
    signal(SIGFPE,sighandler);
    signal(SIGSEGV,sighandler);
}

SYS::~SYS()
{
    del(SUBDB_ID);
}

string SYS::host()
{
    utsname ubuf;
    uname(&ubuf);
    return ubuf.nodename;
}

bool SYS::cfgFileLoad()
{
    int hd = open(mCfgFile.c_str(),O_RDONLY);
    if(hd < 0)
    {
        return false;
    }
    string s_buf;
    int cf_sz = lseek(hd,0,SEEK_END);
    if(cf_sz > 0)
    {
        lseek(hd,SEEK_SET);
        char *buf = (char *)malloc(cf_sz + 1);
        read(hd,buf,cf_sz);
        buf[cf_sz] = 0;
        s_buf = buf;
        free(buf);
    }
    close(hd);


}

/** �������� */
void SYS::taskCreate(const string &path,int priority,void *(start_routine)(void *),void *arg)
{
    int detachStat = 0;
    pthread_t procPthr;
    pthread_attr_t locPAttr, *pthr_attr;

    map<string,STask>::iterator ti;

    ResAlloc res(taskRes, true); /*���� ִ�����Զ�����*/

    for(time_t c_tm = time(NULL); (ti=mTasks.find(path)) != mTasks.end(); )
    {
        //ɾ���Ѵ���������ɵ�δ���ٵ�����
        if(ti->second.flgs&STask::FinishTask && !(ti->second.flgs&STask::Detached))
        {
            pthread_join(ti->second.thr, NULL);
            mTasks.erase(ti);
            continue;
        }

        res.release(); /*�ͷ���*/

        //��ǰ��������
        if(time(NULL) >= (c_tm+5))
            return ;

        usleep(10 * 1000); /*10ms*/

        res.request(true);/*�ټ�����������һ����ѯ*/
    }

    STask &htsk = mTasks[path];
    htsk.path = path;
    htsk.task = start_routine;
    htsk.taskArg = arg;
    htsk.flgs = 0;
    htsk.thr = 0;
    htsk.prior = priority%100; /*0~99*/

    res.release();

//    if(pAttr)
//    {
//        pthr_attr = pAttr;
//    }
//    else
    {
        pthr_attr = &locPAttr;
        pthread_attr_init(pthr_attr); /*Ĭ������*/
    }

    /*�µ��̼̳߳в��ԺͲ���������schedpolicy��schedparam��������ʽ���õĵ�����Ϣ*/
    pthread_attr_setinheritsched(pthr_attr, PTHREAD_EXPLICIT_SCHED);

    struct sched_param prior;
    prior.sched_priority = 0;

    int policy = SCHED_OTHER; /*Ĭ�Ϸ�ʱ����*/

    if(priority > 0)
        policy = SCHED_RR; /*ʱ��Ƭ*/
    if(priority >= 100)
        policy = SCHED_FIFO; /*ʵʱ*/
    pthread_attr_setschedpolicy(pthr_attr, policy);

    prior.sched_priority = vmax(sched_get_priority_min(policy), vmin(sched_get_priority_max(policy),priority%100));

    pthread_attr_setschedparam(pthr_attr, &prior);//���ȼ��趨

    try
    {
        pthread_attr_getdetachstate(pthr_attr,&detachStat);

        if(detachStat == PTHREAD_CREATE_DETACHED)
            htsk.flgs |= STask::Detached; /*�ж��Ƿ��̷߳���*/

        int rez = pthread_create(&procPthr, pthr_attr, taskWrap, &htsk);

        if(rez == EPERM) /*����������*/
        {
            policy = SCHED_OTHER;
            pthread_attr_setschedpolicy(pthr_attr, policy);
            prior.sched_priority = 0;
            pthread_attr_setschedparam(pthr_attr,&prior);

            rez = pthread_create(&procPthr, pthr_attr, taskWrap, &htsk);
        }

        if(pthr_attr)
            pthread_attr_destroy(pthr_attr);

        if(rez)
            throw "Task creation error";

        //�ȴ��߳̽ṹ��ʼ����ɣ�����ɲ��ɷ��������
        while(!(htsk.flgs&STask::Detached) && !htsk.thr)
            usleep(30 * 1000);

//        //�ȴ�����״̬ startSt = NULL
//        for(time_t c_tm = time(NULL); !(htsk.flgs&STask::Detached) && startSt && !(*startSt); )
//        {
//            if(time(NULL) >= (c_tm+wtm))
//                throw "Task start timeouted!";

//            usleep(100 * 1000);
//        }
    }
    catch(const char *msg)
    {
        if(1)  		//ɾ��pthread_create()����Ϣ�������Ժ���ܿ�ʼʱ����������������
        {
            res.request(true);
            mTasks.erase(path);
            res.release();
        }
        throw msg;
    }
}
void SYS::taskDestroy(const string &path,bool *endrunCntr)
{
    map<string,STask>::iterator it;

    ResAlloc res(taskRes, false); /*����ʱ�Զ��ͷ�������*/

    if(mTasks.find(path) == mTasks.end())
        return;

    res.release();/*�ͷ���*/

    if(endrunCntr)
        *endrunCntr = true; /*�����Ѿ����Խ����߳���*/

    //�ȴ��������
    time_t t_tm, s_tm;
    t_tm = s_tm = time(NULL);


    res.request(true); /*д��*/

    while((it=mTasks.find(path)) != mTasks.end() && !(it->second.flgs&STask::FinishTask))
    {
        res.release();

        time_t c_tm = time(NULL);

        //�鿴�Ƿ�ʱ
        if(c_tm > (s_tm+5))
        {
            return ;
        }

        //
        if(c_tm > t_tm+1)
        {
            //1sec
            t_tm = c_tm;
        }
        usleep(100 * 1000);/*100ms*/

        res.request(true);
    }
    if(it != mTasks.end())
    {
        if(!(it->second.flgs&STask::Detached))
            pthread_join(it->second.thr, NULL);
        mTasks.erase(it);
    }
}

void SYS::setTaskName(const char *name)
{
    if(name.empty())
        return ;

    prctl(PR_SET_NAME,name.c_str());
}

void SYS::taskSleep(long long per_ms)
{
    usleep(per_ms * 1000);
}

/** �ַ����������� */
string SYS::strNoSpace(const string &val)
{
    int beg = -1, end = -1;

    for(unsigned int i_s = 0; i_s < val.size(); i_s++)
        if(val[i_s] != ' ' && val[i_s] != '\n' && val[i_s] != '\t')
        {
            if(beg < 0)
                beg = i_s;
            end = i_s;
        }

    return (beg>=0) ? val.substr(beg,end-beg+1) : "";
}

string SYS::strSepParse( const string &path, int level, char sep, int *off)
{
    unsigned int an_dir = off ? *off : 0;
    int t_lev = 0;
    unsigned int t_dir;

    if( an_dir >= path.size() )
        return "";
    while(true)
    {
        t_dir = path.find(sep,an_dir);
        if( t_dir == string::npos )
        {
            if( off )
                *off = path.size();
            return (t_lev == level) ? path.substr(an_dir) : "";
        }
        else if( t_lev == level )
        {
            if( off )
                *off = t_dir+1;
            return path.substr(an_dir,t_dir-an_dir);
        }
        an_dir = t_dir+1;
        t_lev++;
    }
    return "";
}

string SYS::strParse( const string &str, int level, const string &sep, int *off, bool mergeSepSymb)
{
    unsigned int an_dir = off ? *off : 0;
    int t_lev = 0;
    unsigned int t_dir;

    if( an_dir >= path.size() || sep.empty() )
        return "";
    while(true)
    {
        t_dir = path.find(sep,an_dir);
        if( t_dir == string::npos )
        {
            if( off )
                *off = path.size();
            return (t_lev == level) ? path.substr(an_dir) : "";
        }
        else if( t_lev == level )
        {
            if( off )
                *off = t_dir+sep.size();
            return path.substr(an_dir,t_dir-an_dir);
        }
        if( mergeSepSymb && sep.size() == 1 )
            for( an_dir = t_dir; an_dir < path.size() && path[an_dir] == sep[0]; )
                an_dir++;
        else
            an_dir = t_dir+sep.size();
        t_lev++;
    }
    return "";
}
string SYS::strLine( const string &str, int level, int *off)
{
    int an_dir = off ? *off : 0;
    int t_lev = 0, edLnSmbSz = 1;
    size_t t_dir;

    if(an_dir >= (int)str.size())
        return "";
    while(true)
    {
        for(t_dir = an_dir; t_dir < str.size(); t_dir++)
            if(str[t_dir] == '\x0D' || str[t_dir] == '\x0A')
            {
                edLnSmbSz = (str[t_dir] == '\x0D' && ((t_dir+1) < str.size()) && str[t_dir+1] == '\x0A') ? 2 : 1;
                break;
            }
        if(t_dir >= str.size())
        {
            if(off)
                *off = str.size();
            return (t_lev==level) ? str.substr(an_dir) : "";
        }
        else if(t_lev == level)
        {
            if(off)
                *off = t_dir+edLnSmbSz;
            return str.substr(an_dir,t_dir-an_dir);
        }
        an_dir = t_dir+edLnSmbSz;
        t_lev++;
    }
    return "";
}

string SYS::pathLev( const string &path, int level, bool encode, int *off)
{
    unsigned int an_dir = off ? *off : 0;
    int t_lev = 0;
    unsigned int t_dir;

    //- First separators pass -
    while( an_dir<path.size() && path[an_dir]=='/' )
        an_dir++;
    if( an_dir >= path.size() )
        return "";
    //- Path level process -
    while(true)
    {
        t_dir = path.find("/",an_dir);
        if( t_dir == string::npos )
        {
            if( off )
                *off = path.size();
            return (t_lev == level) ? ( encode ? SYS::strDecode(path.substr(an_dir),SYS::PathEl) : path.substr(an_dir) ) : "";
        }
        else if( t_lev == level )
        {
            if( off )
                *off = t_dir;
            return encode ? SYS::strDecode(path.substr(an_dir,t_dir-an_dir),SYS::PathEl) : path.substr(an_dir,t_dir-an_dir);
        }
        an_dir = t_dir;
        t_lev++;
        while( an_dir<path.size() && path[an_dir]=='/' )
            an_dir++;
    }
}
string SYS::path2sepstr( const string &path, char sep)
{
    string rez, curv;
    int off = 0;
    while( !(curv=SYS::pathLev(path,0,false,&off)).empty() )
        rez+=curv+sep;
    if(!rez.empty())
        rez.resize(rez.size()-1);

    return rez;
}

string SYS::sepstr2path( const string &str, char sep)
{
    string rez, curv;
    int off = 0;
    while( !(curv=SYS::strSepParse(str,0,sep,&off)).empty() )
        rez+="/"+curv;

    return rez;
}
string SYS::strEncode( const string &in, SYS::Code tp, const string &symb)
{
    unsigned int i_sz;
    string sout;

    switch(tp)
    {
    case SYS::PathEl:
        sout = in;
        for( i_sz = 0; i_sz < sout.size(); i_sz++ )
            switch( sout[i_sz] )
            {
            case '/':
                sout.replace(i_sz,1,"%2f");
                i_sz+=2;
                break;
            case '%':
                sout.replace(i_sz,1,"%25");
                i_sz+=2;
                break;
            }
        break;
    case SYS::Custom:
        sout.reserve(in.size()+10);
        for( i_sz = 0; i_sz < in.size(); i_sz++ )
        {
            unsigned int i_smb;
            for( i_smb = 0; i_smb < symb.size(); i_smb++ )
                if( in[i_sz] == symb[i_smb] )
                {
                    char buf[4];
                    sprintf(buf,"%%%02X",(unsigned char)in[i_sz]);
                    sout += buf;
                    break;
                }
            if( i_smb >= symb.size() )
                sout+=in[i_sz];
        }
        break;
    case SYS::FormatPrint:
        sout = in;
        for( i_sz = 0; i_sz < sout.size(); i_sz++ )
            if( sout[i_sz] == '%' )
            {
                sout.replace(i_sz,1,"%%");
                i_sz++;
            }
        break;
    }
    return sout;
}
string SYS::strDecode( const string &in, SYS::Code tp)
{
    unsigned int i_sz;
    string sout("");

    switch(tp)
    {
    case SYS::PathEl:
    case SYS::Custom:
        sout.reserve(in.size());
        for( i_sz = 0; i_sz < in.size(); i_sz++ )
            switch( in[i_sz] )
            {
            case '%':
                if( i_sz+2 < in.size() )
                {
                    sout+=(char)strtol(in.substr(i_sz+1,2).c_str(),NULL,16);
                    i_sz+=2;
                }
                else
                    sout+=in[i_sz];
                break;
            default:
                sout+=in[i_sz];
            }
        break;

    default:
        break;
    }

    return sout;
}

/**ת����string*/
string SYS::int2str( int val, IntView view)
{
    char buf[256];
    if(view == SYS::Dec)
        snprintf(buf,sizeof(buf),"%d",val);
    else if(view == SYS::Oct)
        snprintf(buf,sizeof(buf),"%o",val);
    else if(view == SYS::Hex)
        snprintf(buf,sizeof(buf),"%x",val);

    return buf;
}
string SYS::uint2str( unsigned val, IntView view)
{
    char buf[256];
    if(view == SYS::Dec)
        snprintf(buf,sizeof(buf),"%u",val);
    else if(view == SYS::Oct)
        snprintf(buf,sizeof(buf),"%o",val);
    else if(view == SYS::Hex)
        snprintf(buf,sizeof(buf),"%x",val);

    return buf;
}
string SYS::ll2str( long long val, IntView view)
{
    char buf[256];
    if(view == SYS::Dec)
        snprintf(buf,sizeof(buf),"%lld",val);
    else if(view == SYS::Oct)
        snprintf(buf,sizeof(buf),"%llo",val);
    else if(view == SYS::Hex)
        snprintf(buf,sizeof(buf),"%llx",val);

    return buf;
}
string SYS::real2str( double val, int prec, char tp)
{
    char buf[256];
    if(tp == 'g')
        snprintf(buf,sizeof(buf),"%.*g",prec,val);
    else if(tp == 'e')
        snprintf(buf,sizeof(buf),"%.*e",prec,val);
    else
        snprintf(buf,sizeof(buf),"%.*f",prec,val);

    return buf;
}

/** ��ַת�� */
string SYS::addr2str( void *addr )
{
    char buf[sizeof(void*)*2+3];
    snprintf(buf,sizeof(buf),"%p",addr);

    return buf;
}
void * SYS::str2addr( const string &str )
{
    return (void *)strtoul(str.c_str(),NULL,16);
}


void SYS::load_()
{
    static bool first_load = true;

    bool rez = cfgFileLoad(); /* �����ļ����� */

    if(first_load)
    {
        add(new SubDB());
        add(new ModSchedul());

        /* load modules */
        modSchedul().at().load(); /* ÿ���඼�̳��Ը��� ctrlnode �������load�����load_ (�麯��) */
        if(! modSchedul().at().loadLibS())
        {
            stop();
        }

        /* load dbs */
        db().at().load();

        /* ֱ�Ӽ������Ե���ϵͳ��module */
        vector<string> lst;
        list(lst);

        for(unsigned i_a = 0; i_a < lst.size(); i_a++)
        {
            try
            {
                at(lst[i_a]).at().load();
            }
            catch(TError err)
            {

            }
        }
    }
    if(rez)
        stop();
    first_load = false;
}
void SYS::save_() /**/
{

}

long long SYS::curTime()
{
    timeval cur_tm;
    gettimeofday(&cur_tm,NULL);
    return (long long)cur_tm.tv_sec*1000000 + cur_tm.tv_usec;
}

int SYS::start()
{
    vector<string> lst;
    list(lst);

    for(unsigned i_a=0; i_a < lst.size(); i_a++)
        try
        {
            at(lst[i_a]).at().subStart();
        }
        catch(TError err)
        {

        }

    unsigned int i_cnt = 1;
    mStopFlg = 0;
    while(!mStopSignal)
    {
        /* do something */
    }
    /* �ر�ʱ���� ���� */
    save();
    for(int i_a=lst.size()-1; i_a >= 0; i_a--)
        try
        {
            at(lst[i_a]).at().subStop();
        }
        catch(TError err)
        {

        }

    return mStopFlg;
}

void SYS::stop()
{
    mStopFlg = SIGUSR1;
}

void SYS::sighandler(int signal)
{
    switch(signal)
    {
    case SIGINT:
        sys->mStopFlg=signal;
        break;
    case SIGTERM:
        sys->mStopFlg=signal;
        break;
    case SIGFPE:
        exit(1);
        break;
    case SIGCHLD:
    {
        int status;
        pid_t pid = wait(&status);
        if(!WIFEXITED(status) && pid > 0)
            ;
        break;
    }
    case SIGPIPE:
        //mess_warning(SYS->nodePath().c_str(),_("Broken PIPE signal!"));
        break;
    case SIGSEGV:
        //mess_emerg(SYS->nodePath().c_str(),_("Segmentation fault signal!"));
        break;
    case SIGABRT:
        //mess_emerg(SYS->nodePath().c_str(),_("OpenSCADA is aborted!"));
        break;
    case SIGALRM:
        break;
    default:
        //mess_warning(SYS->nodePath().c_str(),_("Unknown signal %d!"),signal);
        break;
    }
}
