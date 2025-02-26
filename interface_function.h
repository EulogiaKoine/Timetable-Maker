#pragma once

// 함수 선언
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // 창 프로시저
void AddRow(HWND);                                   // 새로운 행 추가
void SaveData();                                     // 데이터 저장
void RemoveRow(int, HWND);                           // 행 삭제
void UpdateRowPositions();                           // 행 재배치