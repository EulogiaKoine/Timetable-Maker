#include "scheduleViewer.h"
#include "windowStyler.h"


// 정적 스케줄 목록 관리
static Schedule* schedules = NULL; // 전달된 시간표 데이터
static int scheduleCount = 0;      // 시간표 개수
// 외부에서 위 변수 갱신하는 용
void applySchedules(Schedule* _input, int count){
    schedules = _input;
    scheduleCount = count;
}


// ---------------------------
// ---------- window ---------
// ---------------------------

// 내부적인 화면 관리용 구조체
static struct {
    HWND body;           // 외부 틀
    HWND header;         // 헤더
    HWND sche_container; // 생성된 시간표 컨테이너
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
    // 아래는 기본값; 선택사항
    // wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    // wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = SCHEWIN_BGCOLOR;
    // wc.lpszMenuName = NULL;

    RegisterClassW(&wc);
}

static HWND createOuterFrame(){
    // 화면 우측 상단에 붙게 생성
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    // int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int scrollWidth = GetSystemMetrics(SM_CXVSCROLL); // 스크롤 너비

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
    EndPaint(viewer.header, &ps);
}
static void initWindow(){
    viewer.body = createOuterFrame();
    viewer.header = createHeader();
}


static LRESULT CALLBACK schedule_viewer_procedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_CREATE:
            return 0;

        case WM_DESTROY: // 창 닫기 이벤트
            PostQuitMessage(0); // 프로그램 종료 메시지 전송
            return 0;

        case WM_PAINT:{
            styleHeader();
            return 0;
        }

        // case WM_SIZE:
        //     return 0;


        case WM_SYSCOMMAND:
            // 최대/최소화 명령 무시
            if(wParam == SC_MAXIMIZE || wParam == SC_MINIMIZE)
                return 0;
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}