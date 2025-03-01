#include "windowStyler.h"

void roundRect(HDC hdc, PAINTSTRUCT ps, COLORREF fillColor, COLORREF borderColor, int rad){
    HBRUSH hBrush = CreateSolidBrush(fillColor);
    HPEN hPen = CreatePen(PS_SOLID, 2, borderColor);

    SelectObject(hdc, hBrush);
    SelectObject(hdc, hPen);

    RoundRect(hdc,
        0, 0, ps.rcPaint.right, ps.rcPaint.bottom,
        rad, rad
    );

    DeleteObject(hBrush);
    DeleteObject(hPen);
}

void drawText(HDC hdc, PAINTSTRUCT ps, LPCWSTR text, COLORREF color, bool horizontal_align, bool vertical_align, bool linebreak){
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);

    HFONT hFont = CreateFontW(
        24,                   // 텍스트의 높이 (폰트 크기)  -> 글씨가 위아래로 뭉개지지 않게 방지
        0,                    // 너비는 기본값
        0,                    // 회전 각도는 기본값
        0,                    // 기울이기 각도
        FW_NORMAL,            // 폰트 두께(기본값)
        FALSE, FALSE, FALSE,  // 기울임, 밑줄, 취소선 없음
        DEFAULT_CHARSET,      // 문자셋(기본값: ANSI_DEFAULT)
        OUT_DEFAULT_PRECIS,   // 출력 정밀도(기본값)
        CLIP_DEFAULT_PRECIS,  // 클리핑 정밀도(기본값)
        DEFAULT_QUALITY,      // 품질(기본값)
        DEFAULT_PITCH | FF_DONTCARE, // 글꼴 패밀리
        L"Arial"          // 폰트 이름
    );
    
    SelectObject(hdc, hFont);  // HDC에 새 폰트 적용

    DrawTextW(hdc, text, -1, &ps.rcPaint,
        (horizontal_align? DT_CENTER: DT_LEFT)
        | (vertical_align? DT_VCENTER: DT_TOP)
        | (linebreak? DT_WORDBREAK: DT_SINGLELINE)
    );
}