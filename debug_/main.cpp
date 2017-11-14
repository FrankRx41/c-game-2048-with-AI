#include <stdio.h>
#include <windows.h>
                          
#define _ERROR_LINE_        __LINE__

#undef  _ERROR_MSG_LINE_
#define _ERROR_MSG_LINE_(str,l) MessageBox(NULL,#l": "str,TEXT("Error"),MB_ICONERROR|MB_OK)  
#define ErrorMsg(str)           _ERROR_MSG_LINE_(str,_ERROR_LINE_)


int main(){
    int x = 3;
    if( 1 || (2&&0)){
        printf("1");
    }
    return 0;
}

