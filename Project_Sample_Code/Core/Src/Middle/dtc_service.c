// dtc_service.c

#include "dtc_service.h"
#include <stdio.h>

// CAN 통신 핸들을 사용하기 위해 main.c의 변수를 extern으로 선언
extern CAN_HandleTypeDef hcan1;
// EEPROM 쓰기/읽기 함수가 정의된 헤더를 포함 (실제 구현 시 필요)
// #include "eeprom_25lc256.h" 

// .h 파일에서 extern으로 '선언'했던 전역 변수의 실체를 여기서 '정의'
DTC_Info_t g_currentDTC = {0x0000, false};

// DTC 상태를 확인하고 설정하는 함수 (내용은 이전과 동일)
void DTC_CheckAndSet(uint8_t pmic_fault_status)
{
    if ((pmic_fault_status & 0x01) && (g_currentDTC.isActive == false))
    {
        printf("New UV Fault Detected! Setting DTC...\r\n");
        g_currentDTC.code = 0xC001;
        g_currentDTC.isActive = true;
        // EEPROM_Write_DTC(&g_currentDTC); // 실제 EEPROM 쓰기 함수 호출
    }
}

// DTC를 소거하는 함수
void DTC_Clear(void)
{
    if (g_currentDTC.isActive)
    {
        g_currentDTC.isActive = false;
        g_currentDTC.code = 0x0000;
        printf("DTC Cleared.\r\n");
        // EEPROM_Write_DTC(&g_currentDTC); // 변경된 상태를 EEPROM에 저장
    }
}

/**
  * @brief  수신된 UDS 요청 메시지를 처리하고 응답을 전송하는 함수
  * @param  rx_data: CAN으로 수신된 8바이트 데이터 버퍼의 포인터
  * @retval None
  */
void DTC_ProcessUDSRequest(uint8_t* rx_data)
{
    uint8_t sid = rx_data[1]; // UDS SID는 보통 2번째 바이트에 위치

    CAN_TxHeaderTypeDef tx_header;
    uint8_t tx_data[8] = {0}; // 전송할 데이터를 담을 버퍼
    uint32_t tx_mailbox;

    // 응답 CAN 메시지 헤더 설정
    tx_header.StdId = 0x7E8; // 진단 응답용 표준 ID
    tx_header.ExtId = 0;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;

    switch (sid)
    {
        case 0x19: // SID: ReadDTCInformation (DTC 읽기 요청)
            printf("UDS: Received ReadDTC (0x19) request.\r\n");
            
            tx_header.DLC = 6; // 응답 데이터 길이
            tx_data[0] = 0x06; // UDS 데이터 길이 (6바이트)
            tx_data[1] = 0x59; // Positive Response SID (0x19 + 0x40)
            tx_data[2] = rx_data[2]; // 요청 시 사용된 Sub-function 그대로 사용

            if (g_currentDTC.isActive)
            {
                // DTC가 활성화 상태일 때
                tx_data[3] = (g_currentDTC.code >> 8) & 0xFF; // DTC High Byte
                tx_data[4] = g_currentDTC.code & 0xFF;        // DTC Low Byte
                tx_data[5] = 0x01; // DTC 상태 마스크 (예: testFailed)
            }
            else
            {
                // DTC가 없을 때는 0으로 채움 (혹은 NRC 응답도 가능)
                tx_data[3] = 0x00;
                tx_data[4] = 0x00;
                tx_data[5] = 0x00;
            }
            HAL_CAN_AddTxMessage(&hcan1, &tx_header, tx_data, &tx_mailbox);
            break;

        case 0x14: // SID: ClearDiagnosticInformation (DTC 소거 요청)
            printf("UDS: Received ClearDTC (0x14) request.\r\n");

            DTC_Clear(); // 정의된 DTC 소거 함수 호출

            tx_header.DLC = 2; // 응답 데이터 길이
            tx_data[0] = 0x02; // UDS 데이터 길이 (2바이트)
            tx_data[1] = 0x54; // Positive Response SID (0x14 + 0x40)

            HAL_CAN_AddTxMessage(&hcan1, &tx_header, tx_data, &tx_mailbox);
            break;

        default:
            // 지원하지 않는 SID에 대한 처리
            printf("UDS: Received unsupported SID (0x%02X).\r\n", sid);
            // 실제 차량에서는 Negative Response (NRC)를 보내야 함
            break;
    }
}