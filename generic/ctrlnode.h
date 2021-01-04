/**
控制节点，所有节点的基类
*/


#ifndef __CTRLNODE_H
#define __CTRLNODE_H

#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::make;

class CtrlNode;

typedef map<string,CtrlNode *> TMap;


class CtrlNode
{
public:
    CtrlNode(CtrlNode *iprev = NULL);
    virtual ~CtrlNode();

    virtual CtrlNode &operator=(CtrlNode &node);

    enum Flag
    {
        Flag_NodeEnable = 0x01,  /*使能该节点*/
        Flg_NodeDisable = 0x02,   /*禁用该节点*/
    };

    enum ModifFlag
    {
        None_Modify = 0, /*没有被修改*/
        Self_Modify = 0x01,    /*自身修改*/
        Child_Modify,   /*子节点被修改*/
        All_Modify      /*自身和子节点均被修改*/
    };

    virtual ResRw &UserRes() {return mUserRes;}
    virtual const string &nodeName() = 0; /*节点名称*/
    string nodePath(char sep = 0,bool from_root = false); /*节点路径*/
    /* 节点列表*/
    void nodeList(vector<string> &list,const string &gid = "");
    /* 获取节点*/
    AutoHD<CtrlNode> nodeAt(const string &path,int lev = 0,char sep = 0,int off = 0);
    /* 删除节点 */
    void nodeDel(const string &path,char sep = 0,int flag = 0,bool shDel = 0);

    CtrlNode *nodePrev(bool noex = false);
    Flag nodeFlag() {return mFlg;}
    ModifFlag nodeModifFlag() {return mModifyFlg;}

    /*该节点的引用计数 */
    unsigned nodeUse()  {return mUse;}
    /* 节点的排序顺序*/
    unsigned nodePos() {return mOi;}

    /** 用户操作接口 针对界面操作*/
    bool isModify(int mflg = CtrlNode::All_Modify); /**检查该节点是否被修改*/
    void modif() {mModifyFlg |= Self_Modify;} /*自身被修改*/
    void modifG(); /*整个组被修改*/
    void modifClr() {mModifyFlg = None_Modify;}
    void modifyGClr(); /*组清空 */
    /** end **/


    void load(bool force = false); /*如果节点被修改 加载该节点 */
    void save(); /*如果节点被修改 保存该节点*/

    /**引用计数 */
    virtual void refConnect();
    virtual void refDisConnect();

protected:
    struct GrpEl
    {
        string id;
        bool ordered; /*是否排序*/
        TMap elem;
    };

    void nodeEn(int flag = 0); /*使能节点 */
    void nodeDis(long tm = 0,int flag = 0); /*禁止节点*/

    void nodeDelAll(); /*删除所有节点*/
    void setNodePrev(CtrlNode *node) {prev.node = node;}

    /** 子节点操作 */
    virtual AutoHD<CtrlNode> chldAt(char igr,const string &name,const string &user = "");
    void chldList(char igr,vector<string> &list);

    bool chldPresent(char igr,const string &name); /*该节点是否存在*/
    void chldAdd(char igr,CtrlNode *node,unsigned int pos = -1);
    void chldDel(char igr,const string &name,long tm = -1,int flag = 0,bool shDel = false);

    /** 容器 */
    char grpSize();
    char grpId(const string &sid);
    GrpEl *grpAt(char id);
    unsigned grpAdd(const string &id,bool ordered = false);

    /*在load或save之前做点什么*/
    virtual void load_() {}
    virtual void save_() {}

private:
    struct
    {
        CtrlNode *node;
        char grp;
    }prev; /*节点链表*/
    ResRW mChildRes; /*子节点锁*/
    ResRW mUserRes; /*用户锁 */

    /** 一个 group下可能有多个 子节点 每个子节点可能有多个元素*/
    /** 比如 transport分为 transport_in和 transport_out 组,每个组下有 serial network can 等多个子节点 */
    vector<GrpEl> *chGrp; /** child groups */

    /*当前节点 */
    unsigned short int mUse; /*引用计数*/
    unsigned short int mOi; /*order index 排序*/
    char mFlg;
    char mModifyFlg;
};


#endif
