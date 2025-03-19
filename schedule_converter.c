#include "schedule_converter.h"

static Schedule *g_validSchedules = NULL;
static int g_validScheduleCount = 0;
static int g_validScheduleCapacity = 0;

// --- 유효한 스케줄 조합 추가 함수 ---
// currentComb 배열에 저장된 과목들을 복사하여 하나의 Schedule로 만들고, 전역 배열에 추가함.
static void addValidSchedule(Subject *comb, int count) {
    Schedule sched;
    sched.count = count;
    sched.courses = (Subject *)malloc(sizeof(Subject) * count);
    if (sched.courses == NULL) {
        fwprintf(stderr, L"메모리 할당 실패!\n");
        exit(1);
    }
    memcpy(sched.courses, comb, sizeof(Subject) * count);

    if (g_validScheduleCount == g_validScheduleCapacity) {
        g_validScheduleCapacity = (g_validScheduleCapacity == 0 ? 10 : g_validScheduleCapacity * 2);
        Schedule *temp = (Schedule *)realloc(g_validSchedules, sizeof(Schedule) * g_validScheduleCapacity);
        if (temp == NULL) {
            fwprintf(stderr, L"글로벌 스케줄 배열 재할당 실패!\n");
            exit(1);
        }
        g_validSchedules = temp;
    }
    g_validSchedules[g_validScheduleCount++] = sched;
}

// --- 충돌 검사 함수 ---
// 두 Subject가 같은 요일에 있고, 시간대가 겹치면 1, 아니면 0 반환.
static int conflict(const Subject *a, const Subject *b) {
    if (a->day != b->day) return 0;
    if (a->startTime < b->endTime && b->startTime < a->endTime)
        return 1;
    return 0;
}

// --- 재귀 함수: index번째 과목부터 현재 조합(currentComb)을 확정해 나감 ---
// subjects: 전체 과목 배열, total: 전체 과목 수,
// currentComb: 지금까지 선택된 과목들, currentCount: 그 개수.
static void generateCombinationRecursive(int index, int total, Subject *subjects, Subject *currentComb, int currentCount) {
    if (index == total) {
        // 모든 과목에 대해 결정을 내렸으므로, 현재 조합을 유효 스케줄로 저장.
        addValidSchedule(currentComb, currentCount);
        return;
    }

    // subjects[index]와 현재 조합 내 과목들 사이의 충돌 여부 검사
    int conflictCount = 0;
    int conflictIndices[MAX_ROWS];  // currentComb 내 충돌하는 과목들의 인덱스
    for (int j = 0; j < currentCount; j++) {
        if (conflict(&subjects[index], &currentComb[j])) {
            conflictIndices[conflictCount++] = j;
        }
    }

    if (conflictCount == 0) {
        // 충돌이 없으므로, subjects[index]는 반드시 추가함 (최대 조합을 위해 누락하면 안 됨)
        currentComb[currentCount] = subjects[index];
        generateCombinationRecursive(index + 1, total, subjects, currentComb, currentCount + 1);
    } else {
        // [옵션 1] 새 과목을 스킵 → 기존 결정을 유지
        generateCombinationRecursive(index + 1, total, subjects, currentComb, currentCount);

        // [옵션 2] 충돌하는 기존 과목들을 모두 제거한 후, 새 과목으로 대체하는 경우
        Subject newComb[MAX_ROWS];
        int newCount = 0;
        for (int j = 0; j < currentCount; j++) {
            int isConflict = 0;
            for (int k = 0; k < conflictCount; k++) {
                if (j == conflictIndices[k]) {
                    isConflict = 1;
                    break;
                }
            }
            if (!isConflict) {
                newComb[newCount++] = currentComb[j];
            }
        }
        // subjects[index] 추가
        newComb[newCount] = subjects[index];
        newCount++;
        generateCombinationRecursive(index + 1, total, subjects, newComb, newCount);
    }
}

// --- 요일 파싱 함수 ---
static int parseDay(const wchar_t* dayStr) {
    if (wcscmp(dayStr, L"일") == 0) return 0;
    if (wcscmp(dayStr, L"월") == 0) return 1;
    if (wcscmp(dayStr, L"화") == 0) return 2;
    if (wcscmp(dayStr, L"수") == 0) return 3;
    if (wcscmp(dayStr, L"목") == 0) return 4;
    if (wcscmp(dayStr, L"금") == 0) return 5;
    if (wcscmp(dayStr, L"토") == 0) return 6;
    return -1;
}

// --- 최종 변환 및 스케줄 생성 함수 ---
// courseData를 Subject 배열로 변환한 후, 재귀 함수를 통해 모든 가능한 충돌 없는 과목 조합을 생성함.
// 최종적으로 단 한 번 applySchedules()를 호출하여, 생성된 Schedule 배열과 그 개수를 전달.
void convertAndApply(void) {
    // Course → Subject 변환
    Subject subjects[MAX_ROWS];
    for (int i = 0; i < rowCount; i++) {
        wcsncpy(subjects[i].name, courseData[i].name, 19);
        subjects[i].name[19] = L'\0';
        subjects[i].day = parseDay(courseData[i].day);
        subjects[i].startTime = _wtoi(courseData[i].startTime);
        subjects[i].endTime   = _wtoi(courseData[i].endTime);
    }

    // 전역 유효 스케줄 배열 초기화
    g_validSchedules = NULL;
    g_validScheduleCount = 0;
    g_validScheduleCapacity = 0;

    // 재귀 백트래킹을 위한 임시 배열 (최대 courseData 개수)
    Subject currentComb[MAX_ROWS];
    generateCombinationRecursive(0, rowCount, subjects, currentComb, 0);

    // 최종 생성된 모든 유효 스케줄 조합을 단 한 번 applySchedules()로 전달
    applySchedules(g_validSchedules, g_validScheduleCount);

    // 동적 할당된 메모리 해제 -> 저쪽에서 함
    // for (int i = 0; i < g_validScheduleCount; i++) {
    //     if (g_validSchedules[i].courses != NULL)
    //         free(g_validSchedules[i].courses);
    // }
    // free(g_validSchedules);
}