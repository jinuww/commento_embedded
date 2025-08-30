#ifndef INC_EEPROM_25LC256_H_
#define INC_EEPROM_25LC256_H_

#include "main.h"

// [요구사항] 8바이트 데이터를 Union과 비트필드로 정의
// 1. 상태 플래그들을 위한 비트필드 구조체 (1바이트)
typedef struct {
    uint8_t active    : 1; // 1비트: DTC 활성화 여부 (0: 비활성, 1: 활성)
    uint8_t pending   : 1; // 1비트: Fault 발생 후 대기 상태
    uint8_t confirmed : 1; // 1비트: Fault 확정 상태
    uint8_t reserved  : 5; // 5비트: 예약 공간
} DTC_StatusFlags_t;

// 2. 8바이트 DTC 로그 데이터를 위한 공용체(Union) 정의
typedef union {
    // 해석 방법 1: 구조화된 데이터로 접근
    struct {
        uint16_t          dtc_code;   // 2바이트: 고장 코드 (예: 0x1234)
        DTC_StatusFlags_t status;     // 1바이트: 비트필드 상태 플래그
        uint8_t           reserved;   // 1바이트: 예비 공간
        uint32_t          timestamp;  // 4바이트: 고장 발생 시간
    } structured;

    // 해석 방법 2: 8바이트 배열로 한번에 접근 (SPI/CAN 통신에 매우 유용)
    uint8_t raw_data[8];

} DTC_LogEntry_t;


// 함수 프로토타입 선언
void EEPROM_Write_Log(DTC_LogEntry_t* log_entry);
void EEPROM_Read_Log(DTC_LogEntry_t* log_entry);


#endif /* INC_EEPROM_25LC256_H_ */