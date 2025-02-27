#include <windows.h>
#include <stdio.h>
#include "interface_function.h"

/*
  시간표생성버튼누르면 윈도우창 종료
 */

int main(int argc, char* args){
  showWindow1(GetModuleHandle(NULL));
  return 0;
}