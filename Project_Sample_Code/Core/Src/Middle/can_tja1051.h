#ifndef INC_CAN_TJA1051_H_
#define INC_CAN_TJA1051_H_

#include "main.h"
#include "eeprom_25lc256.h" // DTC 로그 구조체를 사용하기 위해 포함

// 함수 프로토타입 선언
void CAN_Transmit_DTC(DTC_LogEntry_t* log_entry);


#endif /* INC_CAN_TJA1051_H_ */