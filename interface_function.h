#pragma once
// once pragma는 해당 파일에만 적용되기 때문에
// 자칫 여러 번 include될 수 있는 라이브러리는 헤더 파일에서 불러오기
#ifndef UNICODE
    #define UNICODE
#endif
#include <stdio.h>
#include <windows.h>

// 매크로 & 변수 선언 
#define MAX_ROWS 10       // 최대 행 수
#define BTN_ADD 100       // "추가" 버튼 ID
#define BTN_SAVE 101      // "저장" 버튼 ID
#define BTN_REMOVE 200    // "제거" 버튼 ID 
#define BTN_EXIT 300      // "종료 / 시간표 생성" 버튼 ID

HWND hEdit[MAX_ROWS][4];  // 각 행의 강의명, 요일, 시작시간, 끝시간 입력 필드 핸들
HWND hButtonAdd, hButtonSave; // "추가" 및 "저장" 버튼 핸들
HWND hButtonRemove[MAX_ROWS]; // "제거" 버튼 핸들
HWND hButtonExit;             // "종료 / 시간표생성" 버튼 핸들
extern int rowCount;         // 현재 추가된 행 수

// 강의 정보를 저장하는 구조체
typedef struct {
    wchar_t name[100];      // 강의명
    wchar_t day[10];        // 요일
    wchar_t startTime[10];  // 시작시간
    wchar_t endTime[10];    // 끝시간
} Course;

Course courseData[MAX_ROWS]; // 입력된 강의 데이터를 저장할 배열

// 함수 선언
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // 창 프로시저
void showWindow1(HINSTANCE);
void AddRow(HWND);                                   // 새로운 행 추가
void SaveData();                                     // 데이터 저장
void RemoveRow(int, HWND);                           // 행 삭제
void UpdateRowPositions();                           // 행 재배치
void Struct_Saved_Data();							 // 구조체에 데이터 저장