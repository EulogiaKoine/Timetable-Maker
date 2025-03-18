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

void drawText(HDC hdc, PAINTSTRUCT ps, LPCWSTR text, int fontSize, COLORREF color, bool horizontal_align, bool vertical_align, bool linebreak){
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);

    HFONT hFont = CreateFontW(
        fontSize, 0, 0, 0, FW_NORMAL,
        FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial"
    );
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    // 기본 그리기 영역 설정
    RECT rect = ps.rcPaint;
    UINT format = (horizontal_align ? DT_CENTER : DT_LEFT) |
                  (linebreak ? DT_WORDBREAK : DT_SINGLELINE);

    if (linebreak && vertical_align) {
        // 텍스트의 실제 크기를 계산하기 위해 DT_CALCRECT 플래그 사용
        RECT calcRect = rect;
        DrawTextW(hdc, text, -1, &calcRect, format | DT_CALCRECT);
        int textHeight = calcRect.bottom - calcRect.top;
        int totalHeight = rect.bottom - rect.top;
        // 텍스트 높이를 기준으로 수직 중앙 정렬
        int offset = (totalHeight - textHeight) / 2;
        rect.top += offset;
        rect.bottom = rect.top + textHeight;
    } else if (!linebreak && vertical_align) {
        // 단일 라인 모드에서는 DT_VCENTER 플래그 사용 가능
        format |= DT_VCENTER;
    }

    // 텍스트 그리기
    DrawTextW(hdc, text, -1, &rect, format);

    // 리소스 정리
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}


COLORREF hsv2rgb(float h, float s, float v){
    float C = v * s;
    float X = C * (1 - fabs(fmod(h / 60.0, 2) - 1));
    float m = v - C;
    float r_prime, g_prime, b_prime;

    if (h < 60) {
        r_prime = C; g_prime = X; b_prime = 0;
    } else if (h < 120) {
        r_prime = X; g_prime = C; b_prime = 0;
    } else if (h < 180) {
        r_prime = 0; g_prime = C; b_prime = X;
    } else if (h < 240) {
        r_prime = 0; g_prime = X; b_prime = C;
    } else if (h < 300) {
        r_prime = X; g_prime = 0; b_prime = C;
    } else {
        r_prime = C; g_prime = 0; b_prime = X;
    }
    
    int r = (int)((r_prime + m) * 255);
    int g = (int)((g_prime + m) * 255);
    int b = (int)((b_prime + m) * 255);
    return RGB(r, g, b);
}