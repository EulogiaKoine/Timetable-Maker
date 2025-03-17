#pragma once
#ifndef UNICODE // 텍스트 이상 방지
    #define UNICODE
#endif
#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
// GUI 링커 설정이 없다면/콘솔 모드라면 자동으로 진입점을 WinMain으로 설정
// gcc 컴파일 옵션 -mwindows로 대체
// #ifdef _WIN32
//     #ifndef _CONSOLE
//         #pragma comment(linker, "/ENTRY:WinMainCRTStartup")
//     #endif
// #endif



// --------------------------------
// ---------- Interfaces ----------
// --------------------------------

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

// --------------------------------
// ---------- Interfaces ----------
// --------------------------------




// --------------------------------
// ---------- Constants -----------
// --------------------------------

// 스케줄 화면 이름
#define SCHEWIN_CLASSNAME "SCHEDULE_VIEWER"
#define SCHEWIN_CLASSNAME_W L"SCHEDULE_VIEWER"
#define SCHEWIN_TITLE L"Timetable Maker - 뷰어"
#define SCHEHEADER_TITLE L"시간표"

// 화면 크기 옵션
// 초기 설정은 안드로이드 위젯 느낌으로 3:4 or 9:16 or 9:20
// lazy한 탓에 가로세로 이상 비율에 대한 대응 코드 없음. 알아서 잘 조절할 것.
#define SCHEWIN_WIDTH 600 // 가로; 스크롤 반영
#define SCHEWIN_HEIGHT 800 // 세로
#define SCHEWIN_PADDING 10 // 최외곽 패딩
#define SCHEHEADER_HEIGHT 50 // 헤더 높이
#define SCHESEL_WIDTH 200 // 선택창 가로
#define SCHESEL_HIDED_HEIGHT 21 // 드롭다운이 숨겨져 있을 때의 높이(변경 불가)
#define SCHESEL_HEIGHT 100 // 선택창 세로(전체); 50 미만이면 드롭다운이 안됨
#define SCHEMAIN_DAYNAV_HEIGHT 100 // 요일 섹션 높이
#define SCHEMAIN_PERIOD_RATE 0.2 // 교시 섹션 가로 비율
#define SCHEMAIN_TIME_RATE 0.2 // 시간 섹션 가로 비율
#define SCHEMAIN_GAP 0 // 메인 섹션 요소들 간 간격
// 여러 번 쓰는 계산식이라 그냥 매크로로
#define SCHEMAIN_CAL_XPOS \
    ((SCHEWIN_PADDING + ))
#define SCHEMAIN_CAL_WIDTH \
    ((SCHEWIN_WIDTH - SCHEWIN_PADDING*2)*(1-SCHEMAIN_PERIOD_RATE-SCHEMAIN_TIME_RATE)-SCHEMAIN_GAP*2)


// 화면 스타일 옵션
#define SCHEWIN_STYLE (WS_CAPTION \
                        | WS_SYSMENU \
                        | WS_MINIMIZE \
                        | WS_VISIBLE \
                        | WS_TABSTOP \
                        | WS_OVERLAPPED)
#define SCHEWIN_BGCOLOR (HBRUSH)(COLOR_WINDOW+1)

#define SCHEHEADER_STYLE (WS_VISIBLE | WS_CHILD | SS_CENTER)
#define SCHEHEADER_COLOR RGB(108, 190, 191)
#define SCHEHEADER_BORDER_COLOR RGB(255, 255, 255)
#define SCHEHEADER_ROUNDNESS 20  // 괄호 없이, 20px*20px만큼 꺾이도록
#define SCHEHEADER_FONTSIZE 24   // 좀 작지만 예쁜 사이즈

#define SCHESEL_STYLE (WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL) // combobox-dropdownlist: single selection
#define SCHESEL_ALIGN 1 // 수평 정렬 옵션; 0=왼쪽, 1=가운데, 2=오른쪽

#define SCHEMAIN_STYLE (WS_VISIBLE | WS_CHILD)
#define SCHEMAIN_COLOR RGB(229, 228, 226) // Platinum
#define SCHEMAIN_ROUNDNESS 20

#define SCHEMAIN_PERIOD_COLOR RGB(255,255,255)
#define SCHEMAIN_PERIOD_ROUNDNESS SCHEMAIN_ROUNDNESS*1.2
#define SCHEMAIN_PERIOD_FONTSIZE 16
#define SCHEMAIN_PERIOD_FONTCOLOR RGB(0,0,0)

#define SCHEMAIN_TIME_COLOR RGB(255,255,255)
#define SCHEMAIN_TIME_ROUNDNESS SCHEMAIN_ROUNDNESS*1.2
#define SCHEMAIN_TIME_FONTSIZE 16
#define SCHEMAIN_TIME_FONTCOLOR RGB(0,0,0)

#define SCHEMAIN_DAYNAV_LANG 1 // 0=eng, 1=kor
#define SCHEMAIN_DAYNAV_COLOR RGB(255,255,255)
#define SCHEMAIN_DAYNAV_ROUNDNESS 20
#define SCHEMAIN_DAYNAV_FONTSIZE 14
#define SCHEMAIN_DAYNAV_FONTCOLOR RGB(0,0,0)


// 시간표 부분 설정값
// 일단 에브리타임과 동일
#define DEFAULT_START_DAY 1     // 기본 시작 요일 = 월
#define DEFAULT_END_DAY 5       // 기본 끝 요일 = 금
#define DEFAULT_START_TIME 7*60 // 기본 시작 시각 = 7 a.m.
#define DEFAULT_END_TIME 16*60  // 기본 끝 시각 = 4 p.m.
#define PERIOD_OFFSET 9*60      // 1교시 시작시각; 1시간 단위로 +-됨

// 컨트롤 요소 ID, 접두사 ID_ 붙이기
#define ID_SELECTOR (HMENU)1001

// --------------------------------
// ---------- Constants -----------
// --------------------------------