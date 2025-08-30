#ifndef INC_PMIC_MP5475_H_
#define INC_PMIC_MP5475_H_

#include "main.h" // HAL 드라이버 및 기본 정의 포함
#include <stdbool.h>

// [요구사항] Slave Address를 매크로로 정의
#define PMIC_SLAVE_ADDRESS (0x60 << 1) // 7비트 주소를 8비트 형태로 맞추기 위해 왼쪽 시프트

// [요구사항] 레지스터 주소들을 enum으로 정의
typedef enum {
    PMIC_REG_FAULT_STATUS_1 = 0x07, // UV (저전압), OV (과전압) 상태
    PMIC_REG_FAULT_STATUS_2 = 0x08, // OC (과전류) 상태
    PMIC_REG_VOUTA_VREF_HIGH = 0x13, // A채널 전압 제어 (상위 비트)
    PMIC_REG_VOUTA_VREF_LOW  = 0x14  // A채널 전압 제어 (하위 비트)
} PMIC_Register_t;


// 함수 프로토타입 선언
bool PMIC_Read_FaultStatus(uint8_t* fault_status);
void PMIC_Init(void);

// 함수 프로토타입 선언
bool PMIC_Read_FaultStatus(uint8_t* fault_status);
void PMIC_Init(void);
// 전압 변경 함수 선언
bool PMIC_Set_Voltage(uint8_t channel, float voltage);


#endif /* INC_PMIC_MP5475_H_ */