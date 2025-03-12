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
    HWND main;           // 시간표 뷰가 담길 섹션
    HWND daynav;         // 시간표 상단 요일박스
    HWND periodnav;      // 시간표 좌측 교시박스
    HWND calender;       // 시간표 박스
    HWND timenav;        // 시간표 우측 시간박스
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
    // int scrollWidth = GetSystemMetrics(SM_CXVSCROLL); // 스크롤 너비

    return CreateWindowW(
        SCHEWIN_CLASSNAME_W, SCHEWIN_TITLE,
        SCHEWIN_STYLE,
        screenWidth - SCHEWIN_WIDTH - 1 /* - scrollWidth */, 0,
        SCHEWIN_WIDTH /* + scrollWidth */, SCHEWIN_HEIGHT,
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
        SCHEWIN_WIDTH - SCHEWIN_PADDING*2, SCHEHEADER_HEIGHT,
        viewer.body, NULL,
        hInst, NULL
    );
}
static void styleHeader(){
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(viewer.header, &ps);
    roundRect(hdc, ps, SCHEHEADER_COLOR, SCHEHEADER_BORDER_COLOR, SCHEHEADER_ROUNDNESS);
    drawText(hdc, ps, SCHEHEADER_TITLE, SCHEHEADER_FONTSIZE, RGB(0, 0, 0), true, true, false);
    EndPaint(viewer.header, &ps);
}

static HWND createSelector(){
    // 드롭다운형 단일 선택창
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
    wchar_t label[20] = L"";
    for(int i = 0; i < scheduleCount; i++){
        swprintf(label, L"%d번 시간표", i+1);
        SendMessageW(viewer.selector, CB_ADDSTRING, 0,
            (LPARAM)label);
    }
}

static HWND createCalenderContainer(){
    // 사이즈는 남은 공간 전부
    // 채우기 없음
    int y = SCHEWIN_PADDING*2 + SCHEHEADER_HEIGHT + SCHESEL_HIDED_HEIGHT;
    return CreateWindowW(L"STATIC", NULL,
        SCHEMAIN_STYLE,
        SCHEWIN_PADDING, y,
        SCHEWIN_WIDTH - SCHEWIN_PADDING*2, SCHEWIN_HEIGHT - y - SCHEWIN_PADDING*2,
        viewer.body,
        NULL,
        hInst, NULL
    );
}
static void styleCalenderContainer(){
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(viewer.main, &ps);
    roundRect(hdc, ps, SCHEMAIN_COLOR, SCHEMAIN_COLOR, SCHEMAIN_ROUNDNESS);
    EndPaint(viewer.main, &ps);
}

// ~ ~ ~ 캘린더 섹션(main) ~ ~ ~
// 윈도우는 모두 viewer.main의 자식

// schedules를 바탕으로 필요한 요일 범위 반환(dayRange값 변경) -> [시작, 끝]
static void calculateRequiredDays(Schedule template, char dayRange[2]){
    int min = DEFAULT_START_DAY; // 최소 시작
    int max = DEFAULT_END_DAY; // 최대 끝
    for(int i = 0, day; i < template.count; i++){
        day = template.courses[i].day;
        if(day < min)
            min = day;
        if(day > max)
            max = day;
    }
    dayRange[0] = min;
    dayRange[1] = max;
}
// schedules를 바탕으로 필요한 시간 범위 반환(timeRange값 변경) -> [시작, 끝]
static void calculateRequiredTime(Schedule template, int timeRange[2]){
    int min = DEFAULT_START_TIME;
    int max = DEFAULT_END_TIME;
    Subject* temp;
    for(int i = 0; i < template.count; i++){
        temp = &template.courses[i];
        if(temp->startTime < min)
            min = temp->startTime;
        if(temp->endTime > max)
            max = temp->endTime;
    }
    timeRange[0] = min;
    timeRange[1] = max;
}
// timeRange를 교시 범위로 변환
static void convertTimeRangeToPeriodRange(int timeRange[2], int periodRange[2]){
    periodRange[0] = (timeRange[0] - PERIOD_OFFSET)/60 + 1;
    periodRange[1] = (timeRange[1] - PERIOD_OFFSET)/60 + 1;
}

// 좌측 교시 섹션
static HWND createPeriodNav(){
    RECT rect;
    GetWindowRect(viewer.main, &rect);

    return CreateWindowW(L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE,
        0, 0, // main 섹션 내의 상대좌표
        (rect.right - rect.left) * SCHEMAIN_PERIOD_RATE, rect.bottom - rect.top,
        viewer.main,
        NULL,
        hInst, NULL
    );
}
static void markPeriods(Schedule template){ // 교시 표시; 표시할 시간표 기준으로 작동
    if(viewer.periodnav == NULL) return;

    int range[2];
    calculateRequiredTime(template, range);
    convertTimeRangeToPeriodRange(range, range); // 내부 로직을 알아야 가능한 편법이지만 뭐 어때
    
    int periodCount = range[1]-range[0]+1;
    RECT rect;
    GetWindowRect(viewer.main, &rect);
    int offsetY = SCHEMAIN_DAYNAV_HEIGHT + SCHEMAIN_GAP;
    int blockHeight = (rect.bottom - rect.top - offsetY)/periodCount;

    wchar_t periodName[10] = L"";
    for(int i = 0; i < periodCount; i++){
        swprintf(periodName, L"%d교시", i+1);
        CreateWindowW(L"STATIC", (LPCWSTR)periodName,
            WS_CHILD | WS_VISIBLE | SS_RIGHT,
            0, offsetY + blockHeight*i,
            (rect.right - rect.left) * SCHEMAIN_PERIOD_RATE, blockHeight,
            viewer.periodnav,
            NULL,
            hInst, NULL
        );
    }
}


static HWND createDayNav(Schedule template){
    // HWND container = CreateWindowW(L"STATIC", NULL,
    //     WS_CHILD | WS_VISIBLE,
    //     )

    char range[2];
    calculateRequiredDays(template, range);
}



static void listenSelection(WPARAM wparam, LPARAM lparam){ // 시간표 선택 이벤트 리스너
    // ID가 시간표 드롭다운 목록이고(ID_SELECTOR)
    // 콤보박스 항목 변경 이벤트일 경우(CBN_SELCHANGE)
    if(LOWORD(wparam) == (int)ID_SELECTOR && HIWORD(wparam) == CBN_SELCHANGE){
        int idx = SendMessageW((HWND)lparam /* 핸들 겟 */, CB_GETCURSEL, 0, 0);
        if(idx == CB_ERR) return;

        // 선택된 번호에 따른 시간표 띄우기만 하면 끝
        
        // testcode
        // wchar_t selectedText[256];
        // SendMessageW((HWND)lparam, CB_GETLBTEXT, idx, (LPARAM)selectedText);  // 선택된 항목의 텍스트를 가져옴

        // MessageBoxW(NULL, selectedText, L"선택된 값", MB_OK);
    }
}


static void initWindow(){
    viewer.body = createOuterFrame();
    viewer.header = createHeader();

    viewer.selector = createSelector();
    applyChoices();

    viewer.main = createCalenderContainer();
    viewer.periodnav = createPeriodNav();
    Schedule empty = {0};
    markPeriods(empty);
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