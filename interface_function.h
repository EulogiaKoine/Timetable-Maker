#pragma once

//변수선언
#define MAX_ROWS 10       // 최대 행 수
#define BTN_ADD 100       // "추가" 버튼 ID
#define BTN_SAVE 101      // "저장" 버튼 ID
#define BTN_REMOVE 200    // "제거" 버튼 ID

HWND hEdit[MAX_ROWS][4];  // 각 행의 강의명, 요일, 시작시간, 끝시간 입력 필드 핸들
HWND hButtonAdd, hButtonSave; // "추가" 및 "저장" 버튼 핸들
HWND hButtonRemove[MAX_ROWS]; // "제거" 버튼 핸들
int rowCount;         // 현재 추가된 행 수

// 강의 정보를 저장하는 구조체
typedef struct {
    char name[100];      // 강의명
    char day[10];        // 요일
    char startTime[10];  // 시작시간
    char endTime[10];    // 끝시간
} Course;

Course courseData[MAX_ROWS]; // 입력된 강의 데이터를 저장할 배열

// 함수 선언
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // 창 프로시저
int WINAPI showindow1(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow);
void AddRow(HWND);                                   // 새로운 행 추가
void SaveData();                                     // 데이터 저장
void RemoveRow(int, HWND);                           // 행 삭제
void UpdateRowPositions();                           // 행 재배치
void Struct_Saved_Data();							 // 구조체에 데이터 저장