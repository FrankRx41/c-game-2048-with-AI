#ifndef _MACRO_H_
#define _MARCO_H_

#include <stdio.h>
#ifndef _INC_STDIO
extern __declspec(dllimport) int __cdecl printf(const char * _Format, ...);
#endif

#define forp(i,time)        for(int i=0;i<(time);i++)
#define _LOOP_VARNAME_(x,y) var##x##y
#define loop(time)          for(int _LOOP_VARNAME_(__LINE__,__FILE__)=0;        \
                                    _LOOP_VARNAME_(__LINE__,__FILE__)<time;     \
                                    _LOOP_VARNAME_(__LINE__,__FILE__)++)     
#define debug(str,...)      do{                                                 \
                                /*printf("%s(%d):\n",__FILE__,__LINE__);*/      \
                                printf(str"\n",__VA_ARGS__);                    \
                            }while(0)
#define arraylen(a)         (sizeof(a)/sizeof(a[0]))
#define MAX     256
#define strequ(s1,s2)	    (strcmp((s1),(s2))==0)

#define ErrorMsg(str)       MessageBox(NULL,TEXT(str),TEXT("Error"),MB_ICONERROR|MB_OK);

#ifndef _DEBUG
#define debug(str,...)
#endif

#endif