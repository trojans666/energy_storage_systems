/**
 ������,��������node
*/

#ifndef __SYS_H
#define __SYS_H

#include <math.h>

#include <string>
#include <map>

#include "resalloc.h"
#include "ctrlnode.h"
#include "autohd.h"
#include "subsys.h"

#define vmin(a,b)   ((a) < (b) ? (a) : (b))
#define vmax(a,b)   ((a) > (b) ? (a) : (b))

using std::string;
using std::map;



class SYS : public CtrlNode
{
public:
    enum IntView
    {
        Dec, Oct,Hex
    };

    enum Code
    {
        PathEl,Custom,FormatPrint
    };

    SYS(int argi,char **argb,char **env);
    ~SYS();

    int start();
    void stop();

    int stopFlg() {return mStopFlg;}

    int argc_()const {return mArgc;}
    const char **argv()  {return mArgv;}
    const char **envp()  {return mEnvp;}

    const string &id() {return mId;}
    string name() {return mName;}
    void setName(const string &vl) {mName = vl;}
    string user() {return mUser;}
    string host();
    string icoDir() {return mIconDir;} /** ��ȡͼƬ·��*/
    string modDir() {return mModDir;} /** ��ģ��·�� */
    /** �����ļ�·�� */
    string cfgFile() {return mCfgFile;}
    /** �źŴ����� */
    static void sighandler(int signal);
    static long long curTime(); /** ��ǰ��ʱ�� */

#if 0
    AutoHD<TUIS> ui() {return at("UI");}
    AutoHD<TArchive> archive() {return at("Archive");} /** �洢*/
    AutoHD<TBDS> db() {return at("BD");} /** ���ݿ�*/
    AutoHD<TDAQS> daq() {return at("DAQ");} /** �ɼ� */
    AutoHD<TProtocol> protocol() {return at("Protocol");} /** Э�� */
    AutoHD<TTransport> transport() {return at("Transport");} /** ������� */
    AutoHD<TModSchedul> modSchedul() {return at("ModSched");} /** ���� */
#endif

    /** ����ڵ��д洢�������е�subsys��,Ŀ���ǿ���ֱ��ȫ�ַ��� */
    void list(vector<string> &list) { chldList(mSubSt,list);}
    bool present(const string &name) {return chldPresent(mSubSt,name);}
    void add(SubSys *sub) {chldAdd(mSubSt,(CtrlNode*)sub);}
    void del(const string &name) {chldDel(mSubSt,name);}
    AutoHD<SubSys> at(const string &name) {return chldAt(mSubSt,name);}

    /** �������� */
    void taskCreate(const string &path,int priority,void *(start_routine)(void *),void *arg);
    void taskDestroy(const string &path,bool *endrunCntr = NULL);
    static void setTaskName(const char *name);
    static void taskSleep(long long per_ms);

    /** �ַ����������� */
    static string strNoSpace(const string &val); /**������пո� */
    static string strSepParse(const string &path, int level, char sep, int *off = NULL );
    static string strParse( const string &str, int level, const string &sep, int *off = NULL, bool mergeSepSymb = false );
    static string strLine( const string &str, int level, int *off = NULL );
    static string pathLev( const string &path, int level, bool encode = true, int *off = NULL );
    static string path2sepstr( const string &path, char sep = '.' );
    static string sepstr2path( const string &str, char sep = '.' );
    static string strEncode( const string &in, Code tp, const string &symb = " \t\n");
    static string strDecode( const string &in, Code tp = Custom );

    /**ת����string*/
    static string int2str( int val, IntView view = Dec );
    static string uint2str( unsigned val, IntView view = Dec );
    static string ll2str( long long val, IntView view = Dec );
    static string real2str( double val, int prec = 15, char tp = 'g' );
    /** ʵ��ת�� */
    static double realRound( double val, int dig = 0, bool toint = false )
    {
        double rez = floor(val*pow(10,dig)+0.5)/pow(10,dig);
        if( toint ) return floor(rez+0.5);
        return rez;
    }
    /** ��ַת�� */
    static string addr2str( void *addr );
    static void *str2addr( const string &str );

protected:
    void load_();
    void save_(); /*���ڵ㱻save֮ǰ����ʲô*/

private:
    /** ����˽������ */
    class STask
    {
    public:
                    /*����*/          /*����*/
        enum Flgs {Detached = 0x01,FinishTask = 0x02};

        STask():thr(0),policy(0),prior(0),tid(0),flgs(0){}
        STask(pthread_t ithr,char ipolicy,char iprior):thr(ithr),policy(ipolicy),prior(iprior),tid(0),flgs(0){}

        /*����*/
        string          path;/*�߳�·��*/
        pthread_t       thr;/*pid*/
        unsigned char   policy,prior;
        pid_t           tid;
        void *(*task)(void *);
        void            *taskArg;
        unsigned int    flgs;

    };
    map<string,STask> mTasks;
    ResRW taskRes;
    static void *taskWrap(void *stas);

    /** sys��˽������ ���麯�� ����ʵ�� for nodePath()*/
    const string &nodeName() {return id();}


    /** ��ϵͳid */
    int mSubSt;
    int mStopFlg; /*ֹͣ��־ */

    const int mArgc;
    const char **mArgv;
    const char **mEnvp;

    string mUser; /*�û��� */
    string mCfgFile; /*�����ļ�����*/
    string mId; /*վ��id */
    string mName; /*վ������ */
    string mIconDir; /*ͼƬ·��*/
    string mModDir; /*��ģ���·��*/
};

extern SYS *sys;
#endif
