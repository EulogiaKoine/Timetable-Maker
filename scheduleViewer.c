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

void closeWindow2(){
    PostQuitMessage(0);
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
    if(schedules == NULL || scheduleCount == 0) return;
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
    int blockWidth = (rect.right - rect.left) * SCHEMAIN_PERIOD_RATE / 2;

    wchar_t periodName[10] = L"";
    for(int i = 0; i < periodCount; i++){
        swprintf(periodName, L"%d교시", i+1);
        CreateWindowW(L"STATIC", (LPCWSTR)periodName,
            WS_CHILD | WS_VISIBLE | SS_RIGHT,
            blockWidth, offsetY + blockHeight*i - (i/3),
            blockWidth, blockHeight,
            viewer.periodnav,
            NULL,
            hInst, NULL
        );
    }
}
static void stylePeriods(){
    // 바탕(섹션 시각화용)
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(viewer.periodnav, &ps);
    roundRect(hdc, ps, SCHEMAIN_PERIOD_COLOR, SCHEMAIN_PERIOD_COLOR, SCHEMAIN_PERIOD_ROUNDNESS);
    EndPaint(viewer.periodnav, &ps);

    // 글씨
    HWND htxt = NULL; wchar_t title[20];
    while((htxt = FindWindowExW(viewer.periodnav, htxt, L"STATIC", NULL)) != NULL){
        hdc = BeginPaint(htxt, &ps);
        GetWindowTextW(htxt, title, 20);
        drawText(hdc, ps, title, SCHEMAIN_PERIOD_FONTSIZE, SCHEMAIN_PERIOD_FONTCOLOR,
            true, false, false);
        EndPaint(htxt, &ps);
    }
}

// 가운데 캘린더 섹션
// 어쩌다 보니 요일도 포함
static HWND createCalenderBox(){
    RECT rect;
    GetClientRect(viewer.main, &rect);

    int x = (rect.right - rect.left) * SCHEMAIN_PERIOD_RATE + SCHEMAIN_GAP;
    int width = (rect.right - rect.left) * (1 - SCHEMAIN_PERIOD_RATE - SCHEMAIN_TIME_RATE) - SCHEMAIN_GAP*2;
    return CreateWindowW(L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE,
        x, 0,
        width, rect.bottom - rect.top,
        viewer.main,
        NULL,
        hInst, NULL
    );
}

// 우측 시간 섹션
static HWND createTimeNav(){
    RECT rect;
    GetClientRect(viewer.main, &rect);
    int width = (rect.right - rect.left) * SCHEMAIN_TIME_RATE;

    return CreateWindowW(L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE,
        rect.right - width, 0, // main 섹션 내의 상대좌표
        width, rect.bottom - rect.top,
        viewer.main,
        NULL,
        hInst, NULL
    );
}
static void markTimes(Schedule template){
    if(viewer.timenav == NULL) return;

    int range[2];
    calculateRequiredTime(template, range);
    int periodRange[2];
    convertTimeRangeToPeriodRange(range, periodRange);

    range[0] /= 60;
    range[1] = range[1]/60 + 1; // 어차피 시간 단위로 쓸 거
    int periodCount = periodRange[1] - periodRange[0] + 1;
    
    RECT rect;
    GetWindowRect(viewer.main, &rect);
    int offsetY = SCHEMAIN_DAYNAV_HEIGHT + SCHEMAIN_GAP;
    int blockHeight = (rect.bottom - rect.top - offsetY)/periodCount;
    int blockWidth = (rect.right - rect.left) * SCHEMAIN_TIME_RATE / 2;

    wchar_t timeName[20] = L"";
    for(int i = range[0]; i <= range[1]; i++){
        if(i < 12)
            swprintf(timeName, L"%d시", i);
        else
            swprintf(timeName, L"%d시", (i-1) % 12 + 1);
        // swprintf(timeName, L"%d:00", i);
        CreateWindowW(L"STATIC", (LPCWSTR)timeName,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            0, offsetY + blockHeight*(i-range[0]) - (i/3),
            blockWidth, blockHeight,
            viewer.timenav,
            NULL,
            hInst, NULL
        );
    }
}
static void styleTime(){
    // 바탕(섹션 시각화용)
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(viewer.timenav, &ps);
    roundRect(hdc, ps, SCHEMAIN_TIME_COLOR, SCHEMAIN_TIME_COLOR, SCHEMAIN_TIME_ROUNDNESS);
    EndPaint(viewer.timenav, &ps);

    // 글씨
    HWND htxt = NULL; wchar_t title[20];
    while((htxt = FindWindowExW(viewer.timenav, htxt, L"STATIC", NULL)) != NULL){
        hdc = BeginPaint(htxt, &ps);
        GetWindowTextW(htxt, title, 20);
        drawText(hdc, ps, title, SCHEMAIN_TIME_FONTSIZE, SCHEMAIN_TIME_FONTCOLOR,
            true, false, false);
        EndPaint(htxt, &ps);
    }
}

// 캘린더 상단 요일 섹션
static HWND createDayNav(){
    RECT rect;
    GetClientRect(viewer.calender, &rect);

    return CreateWindowW(L"STATIC", NULL,
        WS_CHILD | WS_VISIBLE,
        0, 0,
        rect.right - rect.left, SCHEMAIN_DAYNAV_HEIGHT,
        viewer.calender,
        NULL,
        hInst, NULL
    );
}
static void markDays(Schedule template){
    if(viewer.daynav == NULL) return;

    static wchar_t* dayNames_eng[7] = {
        L"Sun", L"Mon", L"Tues", L"Wed", L"Thur", L"Fri", L"Sat"
    };
    static wchar_t* dayNames_kor[7] = {
        L"일", L"월", L"화", L"수", L"목", L"금", L"토"
    };

    char range[2];
    calculateRequiredDays(template, range);
    RECT rt;
    GetWindowRect(viewer.daynav, &rt);
    int count = range[1] - range[0] + 1;
    int width = (rt.right - rt.left)/count;
    wchar_t dayName[20] = L"";
    wchar_t** dayNamespace = (SCHEMAIN_DAYNAV_LANG==0? dayNames_eng: dayNames_kor);
    for(int i = 0; i < count; i++){
        swprintf(dayName, L"%ws", dayNamespace[range[0]+i]);
        CreateWindowW(L"STATIC", (LPCWSTR)dayName,
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            width * i, SCHEMAIN_DAYNAV_HEIGHT - SCHEMAIN_DAYNAV_FONTSIZE*1.5,
            width, SCHEMAIN_DAYNAV_FONTSIZE,
            viewer.daynav,
            NULL,
            hInst, NULL
        );
    }
}
static void styleDays(){ // 작업중
    if(viewer.daynav == NULL) return;
    // 바탕(섹션 시각화용)
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(viewer.daynav, &ps);
    roundRect(hdc, ps, SCHEMAIN_DAYNAV_COLOR, SCHEMAIN_DAYNAV_COLOR, SCHEMAIN_DAYNAV_ROUNDNESS);
    EndPaint(viewer.daynav, &ps);

    // 글씨
    HWND htxt = NULL; wchar_t title[20];
    while((htxt = FindWindowExW(viewer.daynav, htxt, L"STATIC", NULL)) != NULL){
        hdc = BeginPaint(htxt, &ps);
        GetWindowTextW(htxt, title, 20);
        drawText(hdc, ps, title, SCHEMAIN_DAYNAV_FONTSIZE, SCHEMAIN_DAYNAV_FONTCOLOR,
            true, true, false);
        EndPaint(htxt, &ps);
    }
}

// 캘린더 파트
static COLORREF* colorPalette;
static COLORREF generatePastelRGB(){
    return hsv2rgb(
        (float)(rand() % 360),
        0.2f + ((float)rand() / RAND_MAX) * 0.4f,
        1.f
    );
}
static void initColorPalette(int count){
    if(colorPalette != NULL)
        free(colorPalette);
    colorPalette = (COLORREF*)calloc(count, sizeof(COLORREF));

    int total, r, g;
    srand(time(NULL));
    while(count--)
        colorPalette[count] = generatePastelRGB();
}
static void renderCalender(Schedule template){
    if(viewer.calender == NULL) return;

    // 0. viewer.calender 내 y오프셋 = SCHEMAIN_DAYNAV_HEIGHT + SCHEMAIN_GAP
    static int yOffset = SCHEMAIN_DAYNAV_HEIGHT + SCHEMAIN_GAP;
    // 1. 요일 범위만큼 가로 길이 계산
    // 2. 분당 높이 계산
    // 3. 과목마다 시작 시각으로 행(y) 시작좌표+세로 길이 계산
    // 4. 젠장할 배치

    // 1
    char days[2];
    calculateRequiredDays(template, days);
    RECT rt;
    GetClientRect(viewer.calender, &rt);
    int blockWidth = (rt.right - rt.left) / (days[1]-days[0]+1);

    // 2
    int times[2];
    calculateRequiredTime(template, times);
    int timeLong = ceil((times[1] - times[0]) / 60.0) * 60;
    double blockHeightPerMinute = (rt.bottom - rt.top - yOffset) / (double)timeLong;

    // 3, 4
    Subject *course; wchar_t subInfo[20];
    for(int i = 0; i < template.count; i++){
        course = &template.courses[i];
        if(course == NULL) continue;
        wcscpy(subInfo, course->name);
        CreateWindowW(L"STATIC", (LPCWSTR)subInfo,
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            4 + blockWidth * (course->day - days[0]), yOffset + blockHeightPerMinute * (course->startTime - times[0]),
            blockWidth, blockHeightPerMinute * (course->endTime - course->startTime),
            viewer.calender,
            NULL,
            hInst, NULL
        );
    }
}
static void styleCalender(){
    if(viewer.calender == NULL) return;

    // 첫 요소(daynav) 제외한 블럭들 대상
    HWND hsub = FindWindowExW(viewer.calender, NULL, L"STATIC", NULL); wchar_t title[20];
    PAINTSTRUCT ps; HDC hdc; int i = 0;
    while((hsub = FindWindowExW(viewer.calender, hsub, L"STATIC", NULL)) != NULL){
        hdc = BeginPaint(hsub, &ps);
        GetWindowTextW(hsub, title, 20);
        roundRect(hdc, ps, colorPalette[i++], colorPalette[i], SCHEMAIN_SUBJECT_ROUNDNESS);
        drawText(hdc, ps, title, SCHEMAIN_SUBJECT_FONTSIZE, RGB(0,0,0),
            true, true, true);
        EndPaint(hsub, &ps);
    }
}

// 배경 점선
static Schedule currentSchedule = {0}; // 이제... 클린 코드고 뭐고. 최종이겠지... 이게... 응, 괜찮을 거야......
static void drawGridLines(HDC hdc, RECT rect) {
    if(currentSchedule.count <= 0) return; // 시간표가 없으면 그리지 않음

    int times[2];
    calculateRequiredTime(currentSchedule, times);
    int span = times[1] - times[0];
    int timeLong = (int)(ceil(span / 60.0) * 60);

    // 캘린더 내부에서 요일 네비게이션 아래 시작
    int yOffset = SCHEMAIN_DAYNAV_HEIGHT + SCHEMAIN_GAP;
    int availableHeight = (rect.bottom - rect.top) - yOffset;
    double blockHeightPerMinute = (double)availableHeight / timeLong;

    // 옅은 회색 점선 펜 생성
    HPEN hPen = CreatePen(PS_DOT, 1, RGB(200, 200, 200));
    HPEN hPenOld = (HPEN)SelectObject(hdc, hPen);

    // 60분 단위로 가로줄 그리기 (상단 가장자리 제외)
    for (int t = times[0] + 60; t < times[0] + timeLong; t += 60) {
        int y = yOffset + (int)((t - times[0]) * blockHeightPerMinute);
        MoveToEx(hdc, rect.left, y, NULL);
        LineTo(hdc, rect.right, y);
    }

    SelectObject(hdc, hPenOld);
    DeleteObject(hPen);
}


static void clearStaticChildren(HWND hwnd){
    HWND hChild = NULL;
    while((hChild = FindWindowExW(viewer.periodnav, hChild, L"STATIC", NULL)) != NULL)
        DestroyWindow(hChild);
}
static void tryClearMain(){
    if(viewer.periodnav != NULL){
        clearStaticChildren(viewer.periodnav);
        DestroyWindow(viewer.periodnav);
        viewer.periodnav = NULL;
    }
    if(viewer.calender != NULL){
        if(viewer.daynav != NULL){
            clearStaticChildren(viewer.daynav);
            DestroyWindow(viewer.daynav);
            viewer.daynav = NULL;
        }
        clearStaticChildren(viewer.calender);
        DestroyWindow(viewer.calender);
        viewer.calender = NULL;
    }
    if(viewer.timenav != NULL){
        clearStaticChildren(viewer.timenav);
        DestroyWindow(viewer.timenav);
        viewer.timenav = NULL;
    }
}
static void listenSelection(WPARAM wparam, LPARAM lparam){ // 시간표 선택 이벤트 리스너
    // ID가 시간표 드롭다운 목록이고(ID_SELECTOR)
    // 콤보박스 항목 변경 이벤트일 경우(CBN_SELCHANGE)
    if(LOWORD(wparam) == (int)ID_SELECTOR && HIWORD(wparam) == CBN_SELCHANGE){
        int idx = SendMessageW((HWND)lparam /* 핸들 겟 */, CB_GETCURSEL, 0, 0);
        if(idx == CB_ERR) return;

        // testcode
        // wchar_t selectedText[256];
        // SendMessageW((HWND)lparam, CB_GETLBTEXT, idx, (LPARAM)selectedText);  // 선택된 항목의 텍스트
        // swprintf(selectedText, L"%ws / %d번", selectedText, idx);
        // MessageBoxW(NULL, selectedText, L"선택된 값", MB_OK);

        // main 비우기
        tryClearMain();

        // 선택된 번호에 따른 시간표 띄우기
        viewer.periodnav = createPeriodNav();
        viewer.calender = createCalenderBox();
        viewer.timenav = createTimeNav();
        viewer.daynav = createDayNav();

        Schedule template = schedules[idx];
        markPeriods(template);
        markTimes(template);
        markDays(template);
        renderCalender(template);
        initColorPalette(template.count);
        currentSchedule = template;

        // 강제 즉각 갱신
        InvalidateRect(viewer.body, NULL, TRUE);
    }
}


static void test();
static void initWindow(){
    viewer.body = createOuterFrame();
    viewer.header = createHeader();

    viewer.selector = createSelector();
    applyChoices();

    viewer.main = createCalenderContainer();

    // 테스트
    test();
}
static void test(){
    Subject* courses = (Subject*)calloc(6, sizeof(Subject));
    wcscpy(courses[0].name, L"이산수학");
    courses[0].day = 5;
    courses[0].startTime = 540;
    courses[0].endTime = 770;
    wcscpy(courses[1].name, L"선형대수학");
    courses[1].day = 5;
    courses[1].startTime = 780;
    courses[1].endTime = 900;
    wcscpy(courses[2].name, L"해석학");
    courses[2].day = 1;
    courses[2].startTime = 555;
    courses[2].endTime = 1030;
    wcscpy(courses[3].name, L"소프트웨어공학");
    courses[3].day = 2;
    courses[3].startTime = 540;
    courses[3].endTime = 770;
    wcscpy(courses[4].name, L"알고리즘");
    courses[4].day = 3;
    courses[4].startTime = 780;
    courses[4].endTime = 900;
    wcscpy(courses[5].name, L"자바프로그래밍");
    courses[5].day = 4;
    courses[5].startTime = 555;
    courses[5].endTime = 1030;
    Schedule testTemplate = {
        courses, 6
    };

    Subject* courses2 = (Subject*)calloc(3, sizeof(Subject));
    wcscpy(courses2[0].name, L"소프트웨어공학");
    courses2[0].day = 2;
    courses2[0].startTime = 540;
    courses2[0].endTime = 770;
    wcscpy(courses2[1].name, L"알고리즘");
    courses2[1].day = 0;
    courses2[1].startTime = 780;
    courses2[1].endTime = 900;
    wcscpy(courses2[2].name, L"자바프로그래밍");
    courses2[2].day = 6;
    courses2[2].startTime = 555;
    courses2[2].endTime = 1030;
    Schedule testTemplate2 = {
        courses2, 3
    };

    schedules = (Schedule*)calloc(2, sizeof(Schedule));
    schedules[0] = testTemplate;
    schedules[1] = testTemplate2;
    scheduleCount = 2;
    applyChoices();

    // markPeriods(testTemplate);
    // markTimes(testTemplate);
    // markDays(testTemplate);
    // renderCalender(testTemplate);
    // initColorPalette(testTemplate.count);

    // free(courses);
    // free(schedules);
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

            styleCalenderContainer();
            stylePeriods();
            styleTime();

            // viewer.calender 하위, 부모 먼저 칠해야 자식도 적용됨
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(viewer.calender, &ps);
            FillRect(hdc, &ps.rcPaint, CreateSolidBrush(SCHEMAIN_COLOR));
            drawGridLines(hdc, ps.rcPaint);
            EndPaint(viewer.calender, &ps);
            styleDays();
            styleCalender();
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