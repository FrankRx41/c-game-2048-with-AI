#ifndef _MACRO_H_
#define _MARCO_H_

#define forloop(i,time)     for(int i=0;i<(time);i++)
#define _LOOP_VARNAME_(x)   var##x
#define loop(time)          for(int _LOOP_VARNAME_(__LINE__)=0;_LOOP_VARNAME_(__LINE__)<time;_LOOP_VARNAME_(__LINE__)++)
#define debug(str,...)  do{/*printf("[%d]",__LINE__);*/printf(str"\n",__VA_ARGS__);}while(0)
#define MAX     256
#define strequ(s1,s2)	(strcmp((s1),(s2))==0)


#endif