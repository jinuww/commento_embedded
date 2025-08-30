#ifndef INC_DTC_SERVICE_H_
#define INC_DTC_SERVICE_H_

#include "main.h"
#include <stdbool.h>

// DTC 정보를 담을 구조체 정의
typedef struct {
    uint16_t code;
    bool isActive;
} DTC_Info_t;

// [extern 적용] DTC 정보를 담을 전역 변수를 '선언'. 실체는 .c 파일에 있음
extern DTC_Info_t g_currentDTC;

// DTC 관련 기능 함수 선언
void DTC_CheckAndSet(uint8_t pmic_fault_status);
void DTC_Clear(void);

#endif /* INC_DTC_SERVICE_H_ */