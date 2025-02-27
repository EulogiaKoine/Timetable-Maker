#include <stdio.h>
#include <windows.h>
#include "interface_function.h"

extern int rowCount = 0;         // ���� �߰��� �� ��

// ���α׷� ������
int WINAPI showindow1(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    // ������ Ŭ���� ���
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;              // â ���ν��� ����
    wc.hInstance = hInstance;              // �ν��Ͻ� �ڵ�
    wc.lpszClassName = L"TimetableWindow";  // Ŭ���� �̸�
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // ����
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);      // Ŀ�� ����
    RegisterClass(&wc); // Ŭ���� ���

    // ������ ����
    HWND hWnd = CreateWindow(L"TimetableWindow", L"�ð�ǥ ������", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 400,
        NULL, NULL, hInstance, NULL);

    // ������ ǥ��
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    AddRow(hWnd);

    // �޽��� ����
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // Ű���� �޽��� ��ȯ
        DispatchMessage(&msg);  // �޽��� ó��
    }
    return (int)msg.wParam; // ���α׷� ����
}

// â ���ν���: ������ �޽��� ó��
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: // ������ ���� �� ȣ��
        // "�����߰�" ��ư ����
        hButtonAdd = CreateWindow(L"BUTTON", L"�����߰�", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            70, 320, 100, 30, hWnd, (HMENU)BTN_ADD, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        // "����" ��ư ����
        hButtonSave = CreateWindow(L"BUTTON", L"����", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 320, 50, 30, hWnd, (HMENU)BTN_SAVE, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        break;

    case WM_COMMAND: // ��ư Ŭ�� �� ��� ó��
        if (LOWORD(wParam) == BTN_ADD) { // "�߰�" ��ư Ŭ��
            AddRow(hWnd); // ���ο� �� �߰�
        }
        else if (LOWORD(wParam) == BTN_SAVE) { // "����" ��ư Ŭ��
            SaveData(); // ������ ����
            Struct_Saved_Data(); //������ ����
        }
        else if (LOWORD(wParam) >= BTN_REMOVE && LOWORD(wParam) < BTN_REMOVE + MAX_ROWS) { // "����" ��ư Ŭ��    /   1�� ��ư ~ 10�� ��ư�� ID������
            int index = LOWORD(wParam) - BTN_REMOVE; // Ŭ���� ��ưID - �⺻ID200
            RemoveRow(index, hWnd);
        }
        break;

    case WM_DESTROY: // ������ ���� �� ȣ��
        PostQuitMessage(0); // �޽��� ���� ����
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam); // �⺻ �޽��� ó��
}

// ���ο� �� �߰� �Լ�
void AddRow(HWND hWnd) {
    //�ִ� �� ������ ������ �����޼��� ���
    if (rowCount >= MAX_ROWS) {
        MessageBox(hWnd, L"�ִ� �� ���� �ʰ��߽��ϴ�.", L"���", MB_ICONWARNING);
        return;
    }

    int yPos = 10 + (rowCount * 30); // �ùٸ� ��ġ ���

    //���Ǹ� �Է��ʵ� ����
    hEdit[rowCount][0] = CreateWindow(L"EDIT", L"���Ǹ�", WS_BORDER | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
        10, yPos, 150, 25, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
    //���� �Է��ʵ� ����
    hEdit[rowCount][1] = CreateWindow(L"EDIT", L"����", WS_BORDER | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
        170, yPos, 50, 25, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
    //���۽ð� �Է��ʵ� ����
    hEdit[rowCount][2] = CreateWindow(L"EDIT", L"���۽ð�", WS_BORDER | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
        230, yPos, 100, 25, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
    //���ð� �Է��ʵ� ����
    hEdit[rowCount][3] = CreateWindow(L"EDIT", L"���ð�", WS_BORDER | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
        340, yPos, 100, 25, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
    //���� ��ư ����
    hButtonRemove[rowCount] = CreateWindow(L"BUTTON", L"����", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        440, yPos, 50, 25, hWnd, (HMENU)(BTN_REMOVE + rowCount), (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

    rowCount++; // �� ���� ����
}

// �� ���� �Լ�
void RemoveRow(int index, HWND hWnd) {
    if (index < 0 || index >= rowCount) return;

    // ���� �� ����
    DestroyWindow(hEdit[index][0]);
    DestroyWindow(hEdit[index][1]);
    DestroyWindow(hEdit[index][2]);
    DestroyWindow(hEdit[index][3]);
    DestroyWindow(hButtonRemove[index]);

    // ���ŵ� ���� ����� �� ĭ�� ���� �̵�
    for (int i = index; i < rowCount - 1; i++) {
        hEdit[i][0] = hEdit[i + 1][0];
        hEdit[i][1] = hEdit[i + 1][1];
        hEdit[i][2] = hEdit[i + 1][2];
        hEdit[i][3] = hEdit[i + 1][3];
        hButtonRemove[i] = hButtonRemove[i + 1];

        // ���� ��ư ID ������Ʈ
        SetWindowLongPtr(hButtonRemove[i], GWLP_ID, BTN_REMOVE + i);
    }

    // ������ �� ������ ���� (�ߺ� ���� ����)
    hEdit[rowCount - 1][0] = NULL;
    hEdit[rowCount - 1][1] = NULL;
    hEdit[rowCount - 1][2] = NULL;
    hEdit[rowCount - 1][3] = NULL;
    hButtonRemove[rowCount - 1] = NULL;

    rowCount--;  // ��ü �� ���� ����
    UpdateRowPositions();  // ���� ����� ��ġ ������Ʈ
}

//�� ��ġ ��迭 �Լ�
void UpdateRowPositions() {
    for (int i = 0; i < rowCount; i++) {
        int yPos = 10 + (i * 30);
        MoveWindow(hEdit[i][0], 10, yPos, 150, 25, TRUE);
        MoveWindow(hEdit[i][1], 170, yPos, 50, 25, TRUE);
        MoveWindow(hEdit[i][2], 230, yPos, 100, 25, TRUE);
        MoveWindow(hEdit[i][3], 340, yPos, 100, 25, TRUE);
        MoveWindow(hButtonRemove[i], 440, yPos, 50, 25, TRUE);
    }
}


// ������ ���� �Լ�
void SaveData() {
    // save.txt ������ ���� ���� ����
    FILE* file = fopen("save.txt", "w");
    if (file == NULL) {
        MessageBox(NULL, L"������ �� �� �����ϴ�.", L"����", MB_ICONERROR);
        return;
    }

    // ��� ���� �����͸� ���Ͽ� ����
    for (int i = 0; i < rowCount; i++) {
        wchar_t wName[100], wDay[10], wStartTime[10], wEndTime[10]; //�����ڵ� ���� char
        GetWindowText(hEdit[i][0], wName, 100);
        GetWindowText(hEdit[i][1], wDay, 10);
        GetWindowText(hEdit[i][2], wStartTime, 10);
        GetWindowText(hEdit[i][3], wEndTime, 10);

        // �����ڵ� ���ڿ��� ��Ƽ����Ʈ ���ڿ��� ��ȯ
        char name[100], day[10], startTime[10], endTime[10];
        WideCharToMultiByte(CP_ACP, 0, wName, -1, name, sizeof(name), NULL, NULL);
        WideCharToMultiByte(CP_ACP, 0, wDay, -1, day, sizeof(day), NULL, NULL);
        WideCharToMultiByte(CP_ACP, 0, wStartTime, -1, startTime, sizeof(startTime), NULL, NULL);
        WideCharToMultiByte(CP_ACP, 0, wEndTime, -1, endTime, sizeof(endTime), NULL, NULL);

        // ���Ͽ� ���Ŀ� �°� ����
        fprintf(file, "���Ǹ�: %s, ����: %s, ���۽ð�: %s, ���ð�: %s\n", name, day, startTime, endTime);
    }

    // ���� �ݱ�
    fclose(file);

    MessageBox(NULL, L"�����Ͱ� save.txt ���Ͽ� ����Ǿ����ϴ�.", L"�˸�", MB_OK);
}

// ������ ���� �Լ�
void Struct_Saved_Data() {
    // ��� ���� �����͸� courseData �迭�� ����
    for (int i = 0; i < rowCount; i++) {
        GetWindowTextW(hEdit[i][0], courseData[i].name, sizeof(courseData[i].name));    // ���Ǹ� ����
        GetWindowTextW(hEdit[i][1], courseData[i].day, sizeof(courseData[i].day));      // ���� ����
        GetWindowTextW(hEdit[i][2], courseData[i].startTime, sizeof(courseData[i].startTime)); // ���۽ð� ����
        GetWindowTextW(hEdit[i][3], courseData[i].endTime, sizeof(courseData[i].endTime));     // ���ð� ����
    }
}

/*
WinMain �Լ�:

���α׷�����
������ Ŭ������ ����ϰ�, �����츦 ������ �� �޽��� ������ ����

WndProc �Լ�:

������ �޽����� ó���ϴ� â ���ν���
WM_CREATE: ������ ���� �� "�߰�" �� "����" ��ư�� ����
WM_COMMAND: ��ư Ŭ�� �̺�Ʈ�� ó��
WM_DESTROY: ������ ���� �� �޽��� ������ ����

AddRow �Լ�:

���ο� ���� �߰��ϴ� �Լ�
�ִ� �� ���� �ʰ��ϸ� ��� �޽����� ǥ��
�Է� �ʵ� ������ �����ϸ� ���� �޽����� ǥ��

SaveData �Լ�:
*/