#pragma once

// �Լ� ����
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // â ���ν���
void AddRow(HWND);                                   // ���ο� �� �߰�
void SaveData();                                     // ������ ����
void RemoveRow(int, HWND);                           // �� ����
void UpdateRowPositions();                           // �� ���ġ