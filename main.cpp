#include "terror.h"
#include "sys.h"

#include "log.h"

int main(int argc,char *argv[],char *envp[])
{
#if 0
    int rez = 0;

    try
    {
        sys = new SYS(argc,argv,envp);
       // sys->load();

       // rez = sys->start();

       // delete sys;
    }
    catch(TError err)
    {

    }

   // return rez;
#endif // 0
    sys = new SYS(argc,argv,envp);
    printf("jfdsklfjsl");
    mess_debug("ss","kkkkk");
    mess_info("xx","jfdslkjfdslk");

}
