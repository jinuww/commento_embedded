#include "dtc_service.h"
#include "eeprom_25lc256.h" // EEPROM 쓰기 함수를 사용하기 위해 포함
#include <stdio.h>

// [신규] ADC 전압 초과 DTC 코드 정의
#define DTC_BRAKE_OV_FAULT 0xC002

// .h 파일에서 extern으로 '선언'했던 전역 변수의 실체를 여기서 '정의'
DTC_Info_t g_currentDTC = {0x0000, false};

void DTC_CheckAndSet(uint8_t pmic_fault_status)
{
    // UV Fault가 발생했고, 아직 DTC가 활성화되지 않았다면
    if ((pmic_fault_status & 0x01) && (g_currentDTC.isActive == false))
    {
        printf("New UV Fault Detected! Setting DTC...\r\n");
        g_currentDTC.code = 0xC001; // 예시 DTC 코드
        g_currentDTC.isActive = true;
    }
}

void DTC_Clear(void)
{
    g_currentDTC.isActive = false;
    g_currentDTC.code = 0x0000;
    printf("DTC Cleared.\r\n");
    
void DTC_Set_OV_Fault(void)
{
    // 다른 Fault가 활성화되지 않았을 때만 신규 Fault로 기록
    if (g_currentDTC.isActive == false)
    {
        printf("New OV Fault Detected by ADC! Setting DTC...\r\n");
        g_currentDTC.code = DTC_BRAKE_OV_FAULT;
        g_currentDTC.isActive = true;
        // EEPROM_Write_DTC(&g_currentDTC); // 실제 EEPROM 쓰기 함수 호출
    }
}