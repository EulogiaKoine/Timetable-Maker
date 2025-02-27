#include <windows.h>
#include <stdio.h>
#include "interface_function.h"

/*
  시간표생성버튼누르면 윈도우창 종료
 */

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    showindow1(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    return 0;
}