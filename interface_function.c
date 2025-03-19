#include "interface_function.h"
#include "windowStyler.h"

int rowCount = 0;         // 현재 추가된 행 수

// 프로그램 진입점
void showWindow1(HINSTANCE hInstance) {
    // 윈도우 클래스 등록
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProc;              // 창 프로시저 설정
    wc.hInstance = hInstance;              // 인스턴스 핸들
    wc.lpszClassName = L"TimetableWindow";  // 클래스 이름
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // 배경색
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);      // 커서 설정
    RegisterClassW(&wc); // 클래스 등록

    // 윈도우 생성
    HWND hWnd = CreateWindowW(L"TimetableWindow", L"시간표 생성기", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 400,
        NULL, NULL, hInstance, NULL);

    // 윈도우 표시
    ShowWindow(hWnd, SW_SHOWNORMAL /* 기본 띄우기 */);
    UpdateWindow(hWnd);

    // 메시지 루프
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // 키보드 메시지 변환
        DispatchMessage(&msg);  // 메시지 처리
    }

    // return (int)msg.wParam; // 프로그램 종료
}

// 창 프로시저: 윈도우 메시지 처리
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: // 윈도우 생성 시 호출
        // "과목추가" 버튼 생성
        hButtonAdd = CreateWindowW(L"BUTTON", L"과목추가", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            70, 320, 100, 30, hWnd, (HMENU)BTN_ADD, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        // "저장" 버튼 생성
        hButtonSave = CreateWindowW(L"BUTTON", L"저장", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 320, 50, 30, hWnd, (HMENU)BTN_SAVE, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        // "시간표 생성" 버튼 생성 -> 윈도우창 종료역할도 함
        hButtonExit = CreateWindowW(L"BUTTON", L"시간표 생성", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            350, 320, 100, 30, hWnd, (HMENU)BTN_EXIT, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        LoadData(hWnd);
        break;

    case WM_COMMAND: // 버튼 클릭 등 명령 처리
        if (LOWORD(wParam) == BTN_ADD) { // "추가" 버튼 클릭
            AddRow(hWnd); // 새로운 행 추가
        }
        else if (LOWORD(wParam) == BTN_SAVE) { // "저장" 버튼 클릭
            SaveData(); // 데이터 저장
            Struct_Saved_Data(); //데이터 저장
            MessageBoxW(NULL, L"저장되었습니다!", L"알림", MB_OK);
        }
        else if (LOWORD(wParam) >= BTN_REMOVE && LOWORD(wParam) < BTN_REMOVE + MAX_ROWS) { // "제거" 버튼 클릭    /   1행 버튼 ~ 10행 버튼의 ID감지식
            int index = LOWORD(wParam) - BTN_REMOVE; // 클릭된 버튼ID - 기본ID200
            RemoveRow(index, hWnd);
        }
        else if (LOWORD(wParam) == BTN_EXIT) {  // 🔹 시간표생성 버튼 클릭
            SaveData(); // 데이터 저장
            Struct_Saved_Data(); //데이터 저장
            //PostQuitMessage(0);  // 윈도우 종료
            DestroyWindow(hWnd);
        }
        break;

    case WM_DESTROY: // 윈도우 종료 시 호출
        PostQuitMessage(0); // 메시지 루프 종료
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam); // 기본 메시지 처리
}

// 새로운 행 추가 함수
void AddRow(HWND hWnd) {
    //최대 행 개수를 넘을시 오류메세지 출력
    if (rowCount >= MAX_ROWS) {
        MessageBoxW(hWnd, L"최대 행 수를 초과했습니다.", L"경고", MB_ICONWARNING);
        return;
    }

    int yPos = 10 + (rowCount * 30); // 올바른 위치 계산

    //강의명 입력필드 생성
    hEdit[rowCount][0] = CreateWindowW(L"EDIT", L"강의명", WS_BORDER | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
        10, yPos, 150, 25, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
    //요일 입력필드 생성
    hEdit[rowCount][1] = CreateWindowW(L"EDIT", L"요일", WS_BORDER | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
        170, yPos, 50, 25, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
    //시작시간 입력필드 생성
    hEdit[rowCount][2] = CreateWindowW(L"EDIT", L"시작시간", WS_BORDER | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
        230, yPos, 100, 25, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
    //끝시간 입력필드 생성
    hEdit[rowCount][3] = CreateWindowW(L"EDIT", L"끝시간", WS_BORDER | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
        340, yPos, 100, 25, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
    //제거 버튼 생성
    hButtonRemove[rowCount] = CreateWindowW(L"BUTTON", L"제거", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        440, yPos, 50, 25, hWnd, (HMENU)(BTN_REMOVE + rowCount), (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

    rowCount++; // 행 개수 증가
}

// 행 제거 함수
void RemoveRow(int index, HWND hWnd) {
    if (index < 0 || index >= rowCount) return;

    // 기존 행 제거
    DestroyWindow(hEdit[index][0]);
    DestroyWindow(hEdit[index][1]);
    DestroyWindow(hEdit[index][2]);
    DestroyWindow(hEdit[index][3]);
    DestroyWindow(hButtonRemove[index]);

    // 제거된 이후 행들을 한 칸씩 위로 이동
    for (int i = index; i < rowCount - 1; i++) {
        hEdit[i][0] = hEdit[i + 1][0];
        hEdit[i][1] = hEdit[i + 1][1];
        hEdit[i][2] = hEdit[i + 1][2];
        hEdit[i][3] = hEdit[i + 1][3];
        hButtonRemove[i] = hButtonRemove[i + 1];

        // 제거 버튼 ID 업데이트
        SetWindowLongPtr(hButtonRemove[i], GWLP_ID, BTN_REMOVE + i);
    }

    // 마지막 행 포인터 제거 (중복 참조 방지)
    hEdit[rowCount - 1][0] = NULL;
    hEdit[rowCount - 1][1] = NULL;
    hEdit[rowCount - 1][2] = NULL;
    hEdit[rowCount - 1][3] = NULL;
    hButtonRemove[rowCount - 1] = NULL;

    rowCount--;  // 전체 행 개수 감소
    UpdateRowPositions();  // 남은 행들의 위치 업데이트
}

//행 위치 재배열 함수
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


// 데이터 저장 함수 (바이너리 모드로 저장)
void SaveData() {
    FILE* file = fopen("save.bin", "wb"); // 바이너리 쓰기 모드
    if (file == NULL) return;

    for (int i = 0; i < rowCount; i++) {
        // 각 입력 필드의 텍스트를 고정 크기의 버퍼에 저장 (널 포함)
        wchar_t wName[100] = {0}, wDay[10] = {0}, wStartTime[10] = {0}, wEndTime[10] = {0};

        GetWindowTextW(hEdit[i][0], wName, 100);
        GetWindowTextW(hEdit[i][1], wDay, 10);
        GetWindowTextW(hEdit[i][2], wStartTime, 10);
        GetWindowTextW(hEdit[i][3], wEndTime, 10);

        // 각 배열을 그대로 파일에 기록 (바이트 어레이로 저장)
        fwrite(wName, sizeof(wchar_t), 100, file);
        fwrite(wDay, sizeof(wchar_t), 10, file);
        fwrite(wStartTime, sizeof(wchar_t), 10, file);
        fwrite(wEndTime, sizeof(wchar_t), 10, file);
    }

    fclose(file);
}

// 데이터 불러오기 함수 (바이너리 모드로 읽기)
void LoadData(HWND hWnd) {
    FILE* file = fopen("save.bin", "rb"); // 바이너리 읽기 모드
    if (file == NULL) {
         // 저장된 파일이 없으면 기본 행 하나 추가 후 종료
         AddRow(hWnd);
         return;
    }

    rowCount = 0; // 기존 행 수 초기화
    wchar_t wName[100], wDay[10], wStartTime[10], wEndTime[10];
    int rowsLoaded = 0;

    // 저장된 데이터에서 각 행의 정보를 읽어옴
    while (rowsLoaded < MAX_ROWS &&
           fread(wName, sizeof(wchar_t), 100, file) == 100 &&
           fread(wDay, sizeof(wchar_t), 10, file) == 10 &&
           fread(wStartTime, sizeof(wchar_t), 10, file) == 10 &&
           fread(wEndTime, sizeof(wchar_t), 10, file) == 10)
    {
        AddRow(hWnd); // 새 행 추가
        // 읽어온 데이터를 해당 행의 입력 필드에 설정
        SetWindowTextW(hEdit[rowCount - 1][0], wName);
        SetWindowTextW(hEdit[rowCount - 1][1], wDay);
        SetWindowTextW(hEdit[rowCount - 1][2], wStartTime);
        SetWindowTextW(hEdit[rowCount - 1][3], wEndTime);
        rowsLoaded++;
    }
    fclose(file);

    // 저장된 데이터가 하나도 없으면 기본 행 한 줄 추가
    if (rowsLoaded == 0) {
         AddRow(hWnd);
    }
}



// 데이터 저장 함수
void Struct_Saved_Data() {
    // 모든 행의 데이터를 courseData 배열에 저장
    for (int i = 0; i < rowCount; i++) {
        GetWindowTextW(hEdit[i][0], courseData[i].name, sizeof(courseData[i].name));    // 강의명 저장
        GetWindowTextW(hEdit[i][1], courseData[i].day, sizeof(courseData[i].day));      // 요일 저장
        GetWindowTextW(hEdit[i][2], courseData[i].startTime, sizeof(courseData[i].startTime)); // 시작시간 저장
        GetWindowTextW(hEdit[i][3], courseData[i].endTime, sizeof(courseData[i].endTime));     // 끝시간 저장
    }
}





//txt파일에 형식에 맞게 저장된것도 있고
//courseData[i].name, day, starTime, endTime으로 구조체에도 저장되어있음
// interface_function.h헤더파일을 include하면 쓸 수 있을듯
//(=0~9)