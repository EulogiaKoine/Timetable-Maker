#include "interface_function.h"
#include "scheduleViewer.h"

int main(int argc, char* args){
    // 프로그램 핸들
    HINSTANCE hInst = GetModuleHandle(NULL);

    showWindow1(hInst);
    showWindow2(hInst);
    return 0;
}