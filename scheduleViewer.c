#include "scheduleViewer.h"

typedef struct {
    wchar_t name[20]; // 과목명
    int day; // 일월화수목금토 = 0123456
    int startTime; // 24시간 = 1,440분. 0시 0분부터 분 단위로 흐른 시각.
    int endTime;   // startTime과 동일
} Subject;

/**
 * @name Schedule
 * 시간표 구조체.
 */
typedef struct {
    Subject* courses; // 시간표에 들어가는 과목 배열
    int count;     // 과목 수
} Schedule;



// 정적 스케줄 목록 관리
static Schedule* schedules = NULL; // 전달된 시간표 데이터
static int scheduleCount = 0;      // 시간표 개수
static void applySchedules(const Schedule* _input, int count){
    schedules = _input;
    scheduleCount = count;
}



// ---------------------------
// ---------- window ---------
// ---------------------------

static LRESULT CALLBACK schedule_viewer_procedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static HWND createFrame(HINSTANCE hInst);

void showWindow2(HINSTANCE hInst){
    // default window class
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = schedule_viewer_procedure;
    wc.hInstance = hInst;
    wc.lpszClassName = SCHEWIN_NAME;
}