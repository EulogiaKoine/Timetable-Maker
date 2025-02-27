#pragma once
#include<windows.h>
#include<stdlib.h>
#include<stdbool.h>

// GUI 링커 설정이 없다면/콘솔 모드라면 자동으로 진입점을 WinMain으로 설정
#ifdef _WIN32
    #ifndef _CONSOLE
        #pragma comment(linker, "/ENTRY:WinMainCRTStartup")
    #endif
#endif

/**
 * @name Subject
 * 과목 구조체
 * 지웅님 것과 다르니 잘 가공해서 이거에 맞게 넘겨주세요♥️
 */
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

/**
 * 화면을 띄우기 전 가능한 시간표 조합 데이터를 먼저 등록한다.
 * @param availables 가능한 시간표들 배열. 겹치는 경우는 없어야 한다.
 * @param count 가능한 시간표들의 수.
 */
void applySchedules(Schedule* availables, int count);

// 시간표 화면 띄우기
void showWindow2(HINSTANCE);
 
// 시간표 화면 닫기
void closeWindow2(void);

// 스케줄 화면 이름
#define SCHEWIN_CLASSNAME "SCHEDULE_VIEWER"
#define SCHEWIN_TITLE "Timetable Maker - Viewer"

// 화면 크기 옵션
// 초기 설정은 안드로이드 위젯 느낌으로 3:4 or 9:16 or 9:20
#define SCHEWIN_WIDTH 300 // 가로
#define SCHEWIN_HEIGHT 400 // 세로
#define SCHEWIN_PADDING 10 // 최외곽 패딩

// 화면 스타일 옵션
#define SCHEWIN_STYLE (WS_CAPTION \
                        | WS_SYSMENU \
                        | WS_MINIMIZE \
                        | WS_VISIBLE \
                        | WS_VSCROLL \
                        | WS_TABSTOP \
                        | WS_OVERLAPPED)
#define SCHEWIN_BGCOLOR (HBRUSH)(COLOR_WINDOW+1)