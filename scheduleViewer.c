#include "scheduleViewer.h"


// 정적 스케줄 목록 관리
static Schedule* schedules = NULL; // 전달된 시간표 데이터
static int scheduleCount = 0;      // 시간표 개수
void applySchedules(Schedule* _input, int count){
    schedules = _input;
    scheduleCount = count;
}



// ---------------------------
// ---------- window ---------
// ---------------------------

static LRESULT CALLBACK schedule_viewer_procedure(HWND, UINT, WPARAM, LPARAM);

static bool isWindowAlreadyExists(HINSTANCE hInst);
static void registerWindow(HINSTANCE hInst);
static HWND createOuterFrame(HINSTANCE hInst);


#include<stdio.h>
void showWindow2(HINSTANCE hInst){
    // must close prev window first
    if(isWindowAlreadyExists(hInst))
        return;

    // default window class setting
    registerWindow(hInst);
    HWND houter = createOuterFrame(hInst);
    if(houter == NULL){
        printf("window creation failed with error code: %lu", GetLastError());
        return;
    }

    ShowWindow(houter, SW_SHOWNORMAL);
    UpdateWindow(houter);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}




static bool isWindowAlreadyExists(HINSTANCE hInst){
    WNDCLASSW t_wc;
    return (bool)GetClassInfoW(hInst, SCHEWIN_CLASSNAME_W, &t_wc);
}

static void registerWindow(HINSTANCE hInst){
    // assert isWindowAlreadyExists(hInst) == false
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = schedule_viewer_procedure;
    wc.hInstance = hInst;
    wc.lpszClassName = SCHEWIN_CLASSNAME_W;
    // 아래는 기본값; 선택사항
    // wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    // wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = SCHEWIN_BGCOLOR;
    // wc.lpszMenuName = NULL;

    RegisterClassW(&wc);
}

static HWND createOuterFrame(HINSTANCE hInst){
    // 화면 우측 상단에 붙게 생성
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    // int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    return CreateWindowW(
        SCHEWIN_CLASSNAME_W, SCHEWIN_TITLE,
        SCHEWIN_STYLE,
        screenWidth - SCHEWIN_WIDTH - 1, 0,
        SCHEWIN_WIDTH, SCHEWIN_HEIGHT,
        NULL, NULL,
        hInst, NULL
    );
}


static LRESULT CALLBACK schedule_viewer_procedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_DESTROY: // 창 닫기 이벤트
            PostQuitMessage(0); // 프로그램 종료 메시지 전송
            return 0;

        // case WM_PAINT:
        //     {
        //         PAINTSTRUCT ps;
        //         HDC hdc = BeginPaint(hwnd, &ps); // 그리기 컨텍스트 가져오기
        //         FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW+1)); // 배경 채우기
        //         EndPaint(hwnd, &ps);
        //     }
        //     return 0;

        // case WM_SIZE:
        //     return 0;
        
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}