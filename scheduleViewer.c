#include "scheduleViewer.h"


// ���� ������ ��� ����
static Schedule* schedules = NULL; // ���޵� �ð�ǥ ������
static int scheduleCount = 0;      // �ð�ǥ ����
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
    // �Ʒ��� �⺻��; ���û���
    // wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    // wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = SCHEWIN_BGCOLOR;
    // wc.lpszMenuName = NULL;

    RegisterClassW(&wc);
}

static HWND createOuterFrame(HINSTANCE hInst){
    // ȭ�� ���� ��ܿ� �ٰ� ����
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
        case WM_DESTROY: // â �ݱ� �̺�Ʈ
            PostQuitMessage(0); // ���α׷� ���� �޽��� ����
            return 0;

        // case WM_PAINT:
        //     {
        //         PAINTSTRUCT ps;
        //         HDC hdc = BeginPaint(hwnd, &ps); // �׸��� ���ؽ�Ʈ ��������
        //         FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW+1)); // ��� ä���
        //         EndPaint(hwnd, &ps);
        //     }
        //     return 0;

        // case WM_SIZE:
        //     return 0;
        
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}