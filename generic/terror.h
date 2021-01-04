/**
 �쳣��
*/

#ifndef __TERROR_H
#define __TERROR_H

#include <string>

using std::string;

class TError
{
public:
    TError();
    TError(const char *icat,const char *fmt,...);
    TError(int icod,const char *icat,const char *fmt,...);

    static string getStack(); /*��ȡ��ջ����*/
    static string getErrno(); /*��ȡerrno����*/

    int Cod;
    string Cat;
    string Mess;
};

#endif
