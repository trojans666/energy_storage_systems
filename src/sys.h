/**
 控制类,控制所有node
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
    string icoDir() {return mIconDir;} /** 获取图片路径*/
    string modDir() {return mModDir;} /** 子模块路径 */
    /** 配置文件路径 */
    string cfgFile() {return mCfgFile;}
    /** 信号处理函数 */
    static void sighandler(int signal);
    static long long curTime(); /** 当前的时间 */

#if 0
    AutoHD<TUIS> ui() {return at("UI");}
    AutoHD<TArchive> archive() {return at("Archive");} /** 存储*/
    AutoHD<TBDS> db() {return at("BD");} /** 数据库*/
    AutoHD<TDAQS> daq() {return at("DAQ");} /** 采集 */
    AutoHD<TProtocol> protocol() {return at("Protocol");} /** 协议 */
    AutoHD<TTransport> transport() {return at("Transport");} /** 传输介质 */
    AutoHD<TModSchedul> modSchedul() {return at("ModSched");} /** 调度 */
#endif

    /** 这个节点中存储的是所有的subsys类,目的是可以直接全局访问 */
    void list(vector<string> &list) { chldList(mSubSt,list);}
    bool present(const string &name) {return chldPresent(mSubSt,name);}
    void add(SubSys *sub) {chldAdd(mSubSt,(CtrlNode*)sub);}
    void del(const string &name) {chldDel(mSubSt,name);}
    AutoHD<SubSys> at(const string &name) {return chldAt(mSubSt,name);}

    /** 创建任务 */
    void taskCreate(const string &path,int priority,void *(start_routine)(void *),void *arg);
    void taskDestroy(const string &path,bool *endrunCntr = NULL);
    static void setTaskName(const char *name);
    static void taskSleep(long long per_ms);

    /** 字符串操作函数 */
    static string strNoSpace(const string &val); /**清空所有空格 */
    static string strSepParse(const string &path, int level, char sep, int *off = NULL );
    static string strParse( const string &str, int level, const string &sep, int *off = NULL, bool mergeSepSymb = false );
    static string strLine( const string &str, int level, int *off = NULL );
    static string pathLev( const string &path, int level, bool encode = true, int *off = NULL );
    static string path2sepstr( const string &path, char sep = '.' );
    static string sepstr2path( const string &str, char sep = '.' );
    static string strEncode( const string &in, Code tp, const string &symb = " \t\n");
    static string strDecode( const string &in, Code tp = Custom );

    /**转换到string*/
    static string int2str( int val, IntView view = Dec );
    static string uint2str( unsigned val, IntView view = Dec );
    static string ll2str( long long val, IntView view = Dec );
    static string real2str( double val, int prec = 15, char tp = 'g' );
    /** 实数转换 */
    static double realRound( double val, int dig = 0, bool toint = false )
    {
        double rez = floor(val*pow(10,dig)+0.5)/pow(10,dig);
        if( toint ) return floor(rez+0.5);
        return rez;
    }
    /** 地址转换 */
    static string addr2str( void *addr );
    static void *str2addr( const string &str );

protected:
    void load_();
    void save_(); /*当节点被save之前做点什么*/

private:
    /** 任务私有数据 */
    class STask
    {
    public:
                    /*分离*/          /*结束*/
        enum Flgs {Detached = 0x01,FinishTask = 0x02};

        STask():thr(0),policy(0),prior(0),tid(0),flgs(0){}
        STask(pthread_t ithr,char ipolicy,char iprior):thr(ithr),policy(ipolicy),prior(iprior),tid(0),flgs(0){}

        /*属性*/
        string          path;/*线程路径*/
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

    /** sys的私有数据 纯虚函数 必须实现 for nodePath()*/
    const string &nodeName() {return id();}


    /** 子系统id */
    int mSubSt;
    int mStopFlg; /*停止标志 */

    const int mArgc;
    const char **mArgv;
    const char **mEnvp;

    string mUser; /*用户名 */
    string mCfgFile; /*配置文件名称*/
    string mId; /*站点id */
    string mName; /*站点名称 */
    string mIconDir; /*图片路径*/
    string mModDir; /*子模块库路径*/
};

extern SYS *sys;
#endif
