#pragma once

//��������
#define MAX_ROWS 10       // �ִ� �� ��
#define BTN_ADD 100       // "�߰�" ��ư ID
#define BTN_SAVE 101      // "����" ��ư ID
#define BTN_REMOVE 200    // "����" ��ư ID

HWND hEdit[MAX_ROWS][4];  // �� ���� ���Ǹ�, ����, ���۽ð�, ���ð� �Է� �ʵ� �ڵ�
HWND hButtonAdd, hButtonSave; // "�߰�" �� "����" ��ư �ڵ�
HWND hButtonRemove[MAX_ROWS]; // "����" ��ư �ڵ�
int rowCount;         // ���� �߰��� �� ��

// ���� ������ �����ϴ� ����ü
typedef struct {
    char name[100];      // ���Ǹ�
    char day[10];        // ����
    char startTime[10];  // ���۽ð�
    char endTime[10];    // ���ð�
} Course;

Course courseData[MAX_ROWS]; // �Էµ� ���� �����͸� ������ �迭

// �Լ� ����
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // â ���ν���
int WINAPI showindow1(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow);
void AddRow(HWND);                                   // ���ο� �� �߰�
void SaveData();                                     // ������ ����
void RemoveRow(int, HWND);                           // �� ����
void UpdateRowPositions();                           // �� ���ġ
void Struct_Saved_Data();							 // ����ü�� ������ ����