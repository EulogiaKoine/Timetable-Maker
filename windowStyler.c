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
    DrawTextW(hdc, text, -1, &ps.rcPaint,
        (horizontal_align? DT_CENTER: DT_LEFT)
        | (vertical_align? DT_VCENTER: DT_TOP)
        | (linebreak? DT_WORDBREAK: DT_SINGLELINE)
    );
}