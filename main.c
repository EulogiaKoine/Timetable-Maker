#include "scheduleViewer.h"
#include "interface_function.h"
#include "schedule_converter.h"

int main(int argc, char* args) {
    // 프로그램 핸들
    HINSTANCE hInst = GetModuleHandle(NULL);

    showWindow1(hInst);
    convertAndApply();
    showWindow2(hInst);
    return 0;
}