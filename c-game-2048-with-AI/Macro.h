#ifndef _MACRO_H_
#define _MARCO_H_

#define forloop(i,time)     for(int i=0;i<(time);i++)
#define _LOOP_VARNAME_(x,y) var##x##y
#define loop(time)          for(int _LOOP_VARNAME_(__LINE__,__FILE__)=0;        \
                                    _LOOP_VARNAME_(__LINE__,__FILE__)<time;     \
                                    _LOOP_VARNAME_(__LINE__,__FILE__)++)     
#define debug(str,...)      do{                                                 \
                                /*printf("%s(%d)",__FILE__,__LINE__);*/         \
                                printf(str"\n",__VA_ARGS__);                    \
                            }while(0)
#define MAX     256
#define strequ(s1,s2)	    (strcmp((s1),(s2))==0)


#endif