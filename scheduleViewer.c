#include "scheduleViewer.h"
#include "windowStyler.h"


// ???? ?????? ??? ????
static Schedule* schedules = NULL; // ????? ?©£?? ??????
static int scheduleCount = 0;      // ?©£?? ????
// ??¥ï??? ?? ???? ??????? ??
void applySchedules(Schedule* _input, int count){
    schedules = _input;
    scheduleCount = count;
}


// ---------------------------
// ---------- window ---------
// ---------------------------

// ???????? ??? ?????? ?????
static struct {
    HWND body;           // ??? ?
    HWND header;         // ???
    HWND sche_container; // ?????? ?©£?? ???????
} viewer = { false };


HINSTANCE hInst = NULL;
static LRESULT CALLBACK schedule_viewer_procedure(HWND, UINT, WPARAM, LPARAM);

// assert hInst != NULL
static bool isWindowAlreadyExists();
static void registerWindow();
static void initWindow();



void showWindow2(HINSTANCE hInstance){
    if((hInst = hInstance) == NULL)
        return;

    // must close prev window first
    if(isWindowAlreadyExists())
        return;

    // default window class setting
    registerWindow();
    initWindow();

    ShowWindow(viewer.body, SW_SHOWNORMAL);
    UpdateWindow(viewer.body);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}




static bool isWindowAlreadyExists(){
    WNDCLASSW t_wc;
    return (bool)GetClassInfoW(hInst, SCHEWIN_CLASSNAME_W, &t_wc);
}

static void registerWindow(){
    // assert isWindowAlreadyExists(hInst) == false
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = schedule_viewer_procedure;
    wc.hInstance = hInst;
    wc.lpszClassName = SCHEWIN_CLASSNAME_W;
    // ????? ????; ???????
    // wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    // wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = SCHEWIN_BGCOLOR;
    // wc.lpszMenuName = NULL;

    RegisterClassW(&wc);
}

static HWND createOuterFrame(){
    // ??? ???? ???? ??? ????
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    // int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int scrollWidth = GetSystemMetrics(SM_CXVSCROLL); // ????? ???

    return CreateWindowW(
        SCHEWIN_CLASSNAME_W, SCHEWIN_TITLE,
        SCHEWIN_STYLE,
        screenWidth - SCHEWIN_WIDTH - 1 - scrollWidth, 0,
        SCHEWIN_WIDTH + scrollWidth, SCHEWIN_HEIGHT,
        NULL, NULL,
        hInst, NULL
    );
}
static HWND createHeader(){
    return CreateWindowW(
        L"STATIC", SCHEHEADER_TITLE,
        SCHEHEADER_STYLE,
        SCHEWIN_PADDING, SCHEWIN_PADDING,
        280, SCHEHEADER_HEIGHT,
        viewer.body, NULL,
        hInst, NULL
    );
}
static void styleHeader(){
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(viewer.header, &ps);
    roundRect(hdc, ps, SCHEHEADER_COLOR, SCHEHEADER_BORDER_COLOR, SCHEHEADER_ROUNDNESS);
    drawText(hdc, ps, SCHEHEADER_TITLE, RGB(0, 0, 0), true, true, false);
}
static void initWindow(){
    viewer.body = createOuterFrame();
    viewer.header = createHeader();
}


static LRESULT CALLBACK schedule_viewer_procedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_CREATE:
            return 0;

        case WM_DESTROY: // ? ??? ????
            PostQuitMessage(0); // ???¥á?? ???? ????? ????
            return 0;

        case WM_PAINT:{
            styleHeader();
            return 0;
        }

        // case WM_SIZE:
        //     return 0;


        case WM_SYSCOMMAND:
            // ???/???? ???? ????
            if(wParam == SC_MAXIMIZE || wParam == SC_MINIMIZE)
                return 0;
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}