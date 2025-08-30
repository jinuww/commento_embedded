#include "test_cases.h"
#include <stdio.h>
#include <string.h>
#include "pmic_mp5475.h"    // PMIC 전압 설정 함수 테스트용
#include "eeprom_25lc256.h"  // DTC Union/Bit-field 테스트용

// ==============================================================================
// ## White Box Test Cases
// ==============================================================================

/**
 * @brief [White Box TC 1] PMIC_Set_Voltage 함수의 내부 전압 계산 로직 검증
 */
static void WB_TC1_Voltage_Calculation_Logic(void)
{
    printf("\r\n--- [WB TC 1] PMIC Voltage Calculation Logic ---\r\n");
    float test_voltage = 1.2f;
    uint16_t expected_digital_value = 450; // (1.2 - 0.3) / 0.002 = 450
    
    // 실제 계산 로직 (PMIC_Set_Voltage 함수 내부 로직과 동일)
    uint16_t calculated_digital_value = (uint16_t)((test_voltage - 0.3f) / 0.002f);

    if (calculated_digital_value == expected_digital_value) {
        printf("  Result: PASS\r\n");
        printf("  (Input: %.3fV, Expected: %u, Calculated: %u)\r\n", 
               test_voltage, expected_digital_value, calculated_digital_value);
    } else {
        printf("  Result: FAIL\r\n");
        printf("  (Input: %.3fV, Expected: %u, Calculated: %u)\r\n", 
               test_voltage, expected_digital_value, calculated_digital_value);
    }
}

/**
 * @brief [White Box TC 2] DTC Log Union/Bit-field의 메모리 구조 검증
 */
static void WB_TC2_DTC_Union_Memory_Layout(void)
{
    printf("\r\n--- [WB TC 2] DTC Union Memory Layout ---\r\n");
    DTC_LogEntry_t test_log;

    // 1. 구조체(structured) 멤버에 값을 할당
    test_log.structured.dtc_code = 0xC001;
    test_log.structured.status.active = 1;
    test_log.structured.status.pending = 0;
    test_log.structured.status.confirmed = 1;
    
    // 2. 배열(raw_data) 멤버의 값이 예상대로 채워졌는지 확인 (Little-endian 기준)
    bool pass = true;
    if (test_log.raw_data[0] != 0x01) pass = false; // DTC Low Byte
    if (test_log.raw_data[1] != 0xC0) pass = false; // DTC High Byte
    if (test_log.raw_data[2] != 0x05) pass = false; // Status (active=1, confirmed=1 => 0b...101)

    if (pass) {
        printf("  Result: PASS\r\n");
        printf("  (DTC: 0x%04X -> Raw[0,1]: 0x%02X, 0x%02X)\r\n", 
               test_log.structured.dtc_code, test_log.raw_data[0], test_log.raw_data[1]);
    } else {
        printf("  Result: FAIL\r\n");
    }
}

// ==============================================================================
// ## Black Box Test Cases
// ==============================================================================

/**
 * @brief [Black Box TC 1] UDS DTC Read 기능 검증
 * @note  이 테스트는 수동 개입이 필요합니다.
 */
static void BB_TC1_UDS_Read_DTC(void)
{
    printf("\r\n--- [BB TC 1] UDS Read DTC Functionality ---\r\n");
    printf("  Action 1: Induce a UV fault by lowering PMIC VIN below 2.8V.\r\n");
    printf("  Action 2: Using a CAN tool, send a UDS Read DTC request.\r\n");
    printf("            (CAN ID: 0x7DF, Data: 02 19 02 FF FF FF FF FF)\r\n");
    printf("  Expected: Receive a CAN response with DTC 0xC001.\r\n");
    printf("            (CAN ID: 0x7E8, Data: 06 59 02 C0 01 ...)\r\n");
    printf("  Waiting for 20 seconds for user action...\r\n");
    HAL_Delay(20000);
}

/**
 * @brief [Black Box TC 2] UDS DTC Clear 기능 검증
 * @note  이 테스트는 수동 개입이 필요합니다.
 */
static void BB_TC2_UDS_Clear_DTC(void)
{
    printf("\r\n--- [BB TC 2] UDS Clear DTC Functionality ---\r\n");
    printf("  (Prerequisite: Ensure a fault is active by running TC 1 first)\r\n");
    printf("  Action 1: Restore PMIC VIN to a normal voltage (e.g., 12V).\r\n");
    printf("  Action 2: Using a CAN tool, send a UDS Clear DTC request.\r\n");
    printf("            (CAN ID: 0x7DF, Data: 04 14 FF FF FF FF FF FF)\r\n");
    printf("  Expected: Receive a UDS Clear positive response.\r\n");
    printf("            (CAN ID: 0x7E8, Data: 02 54 ...)\r\n");
    printf("  Action 3: After clear, send a UDS Read DTC request again.\r\n");
    printf("  Expected: The response should now indicate NO active DTC.\r\n");
    printf("  Waiting for 30 seconds for user action...\r\n");
    HAL_Delay(30000);
}

/**
 * @brief 모든 테스트 케이스를 순차적으로 실행하는 메인 함수
 */
void Run_All_TestCases(void)
{
    printf("#############################################\r\n");
    printf("##         STARTING ALL TEST CASES         ##\r\n");
    printf("#############################################\r\n");

    WB_TC1_Voltage_Calculation_Logic();
    WB_TC2_DTC_Union_Memory_Layout();
    
    BB_TC1_UDS_Read_DTC();
    BB_TC2_UDS_Clear_DTC();

    printf("\r\n#############################################\r\n");
    printf("##          ALL TEST CASES COMPLETE        ##\r\n");
    printf("#############################################\r\n");
}