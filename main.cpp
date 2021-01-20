#include "terror.h"
#include "sys.h"

#include "log.h"

int main(int argc,char *argv[],char *envp[])
{

    int rez = 0;

    try
    {
        sys = new SYS(argc,argv,envp);
        sys->load();
        /* �������� substart,ÿ��substart������ڲ����е�modstart */
        rez = sys->start();

        delete sys;
    }
    catch(TError err)
    {

    }

   return rez;
}
