// 창 스타일링용 유틸리티 모음

#pragma once
#include<windows.h>
#include<stdbool.h>

// 해당 요소를 둥근 직사각형으로 그리기
// STATIC으로 생성된 요소 권장
void roundRect(
    HDC hdc, PAINTSTRUCT ps,                   // 그릴 요소의 그리기용 컨텍스트와 정보
    COLORREF fillColor, COLORREF borderColor,  // 배경색과 테두리색
    int roundness                              // 둥근 정도; 반지름(px)
);

// 요소 내에 텍스트 그리기
// 정렬 가능
void drawText(
    HDC hdc, PAINTSTRUCT ps,                    // 그릴 요소의 그리기용 컨텍스트와 정보
    LPCWSTR text,                               // 그릴 텍스트
    COLORREF color,                             // 텍스트 색깔
    bool horizontal_align, bool vertical_align, // 수평 가운데 정렬, 수직 가운데 정렬 여부
    bool linebreak                              // 자동 줄바꿈 여부
);