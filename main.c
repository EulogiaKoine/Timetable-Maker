#include <windows.h>
#include <stdio.h>
#include "interface_function.h"

/*
  시간표생성버튼누르면 윈도우창 종료
 */

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    showindow1(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    return 0;
}