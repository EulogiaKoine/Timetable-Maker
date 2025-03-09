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
    HWND selector;       // 생성된 시간표 선택창
    HWND sche_container; // 생성된 시간표 컨테이너
} viewer = { 0 };


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



// ---------------------------
// -------- win init ---------
// ---------------------------

// 스케줄 화면 기준 수평 방향 정렬 옵션에 따른 x좌표 반환; 패딩 적용
// option: 0=left, 1=center, 2=rigth
// width: 배치할 요소의 가로 길이
static int calculateAlignPos(int option, int width){
    switch(option){
        case 0: // 왼쪽 정렬
            return SCHEWIN_PADDING;
        case 1: // 중앙 정렬
            return (SCHEWIN_WIDTH - width) / 2;
        case 2: // 오른쪽 정렬
            return SCHEWIN_WIDTH - width - SCHEWIN_PADDING;
        default:
            return SCHEWIN_PADDING;
    }
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
static void styleOuterFrame(){
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(viewer.body, &ps);
    FillRect(hdc, &ps.rcPaint, SCHEWIN_BGCOLOR);
    EndPaint(viewer.body, &ps);
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

static HWND createSelector(){
    return CreateWindowW(L"COMBOBOX",
        NULL,    // default text
        SCHESEL_STYLE,
        calculateAlignPos(SCHESEL_ALIGN, SCHESEL_WIDTH), (int)SCHEWIN_PADDING*1.2 + SCHEHEADER_HEIGHT,
        SCHESEL_WIDTH, SCHESEL_HEIGHT,
        viewer.body,
        ID_SELECTOR,
        hInst, NULL
    );
}
static void applyChoices(){ // schedules 데이터로부터 선택지 적용 및 생성
    wchar_t label[20];
    for(int i = 0; i < scheduleCount; i++){
        swprintf(label, L"%d번 시간표", i+1);
        SendMessageW(viewer.selector, CB_ADDSTRING, 0,
            (LPARAM)label);
    }
}
static void listenSelection(WPARAM wparam, LPARAM lparam){ // 시간표 선택 이벤트 리스너
    // ID가 시간표 드롭다운 목록이고(ID_SELECTOR)
    // 콤보박스 항목 변경 이벤트일 경우(CBN_SELCHANGE)
    if(LOWORD(wparam) == (int)ID_SELECTOR && HIWORD(wparam) == CBN_SELCHANGE){
        int idx = SendMessageW((HWND)lparam /* 핸들 겟 */, CB_GETCURSEL, 0, 0);
        if(idx == CB_ERR) return;

        // 선택된 번호에 따른 시간표 띄우기만 하면 끝
        
        // testcode
        wchar_t selectedText[256];
        SendMessageW((HWND)lparam, CB_GETLBTEXT, idx, (LPARAM)selectedText);  // 선택된 항목의 텍스트를 가져옴

        MessageBoxW(NULL, selectedText, L"선택된 값", MB_OK);
    }
}


static void initWindow(){
    viewer.body = createOuterFrame();
    viewer.header = createHeader();
    viewer.selector = createSelector();
    scheduleCount = 7;
    applyChoices();
}


// ---------------------------
// -------- win init ---------
// ---------------------------




// ---------------------------
// -------- procedure --------
// ---------------------------
static LRESULT CALLBACK schedule_viewer_procedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_CREATE: {
            break;
        }
        case WM_DESTROY: // 창 닫기 이벤트
            PostQuitMessage(0); // 프로그램 종료 메시지 전송
            return 0;

        case WM_PAINT:{
            styleOuterFrame();
            styleHeader();
            break;
        }

        case WM_INITDIALOG:
            break;


        // case WM_SIZE:
        //     return 0;

        case WM_COMMAND:
            listenSelection(wParam, lParam);
            break;

        case WM_SYSCOMMAND:
            // 최대/최소화 명령 무시
            if(wParam == SC_MAXIMIZE || wParam == SC_MINIMIZE)
                return 0;
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}