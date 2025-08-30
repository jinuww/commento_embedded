#include "pmic_mp5475.h"
#include <stdio.h>

// main.c에 정의된 I2C 핸들 변수를 사용하기 위해 extern으로 선언
extern I2C_HandleTypeDef hi2c1;

/**
  * @brief  PMIC 칩 초기화 및 통신 확인
  * @param  None
  * @retval None
  */
void PMIC_Init(void)
{
    // HAL_I2C_IsDeviceReady: I2C 버스에 해당 주소의 장치가 응답하는지 확인
    // 5번 시도하여 100ms 안에 응답이 오는지 체크
    if (HAL_I2C_IsDeviceReady(&hi2c1, PMIC_SLAVE_ADDRESS, 5, 100) == HAL_OK)
    {
        printf("PMIC (MP5475) is ready.\r\n");
    }
    else
    {
        printf("Error: PMIC (MP5475) not found.\r\n");
    }
}

/**
  * @brief  PMIC의 Fault 상태 레지스터(0x07)를 읽어옴
  * @param  fault_status: 읽어온 레지스터 값을 저장할 포인터
  * @retval bool: Fault가 하나라도 감지되면 true, 아니면 false
  */
bool PMIC_Read_FaultStatus(uint8_t* fault_status)
{
    // HAL_I2C_Mem_Read: 특정 메모리 주소(레지스터)의 값을 읽어오는 함수 (Polling 방식)
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, PMIC_SLAVE_ADDRESS, PMIC_REG_FAULT_STATUS_1, I2C_MEMADD_SIZE_8BIT, fault_status, 1, HAL_MAX_DELAY);

    if (status == HAL_OK)
    {
        // 통신 성공 시, 읽어온 값이 0이 아니면 Fault가 발생한 것
        if (*fault_status != 0x00)
        {
            return true; // Fault 감지됨
        }
        return false; // Fault 없음
    }
    else
    {
        // 통신 실패 시
        *fault_status = 0xFF; // 에러 값
        printf("Error: Failed to read PMIC Fault Status.\r\n");
        return false;
    }
}

/**
  * @brief  PMIC의 특정 채널 출력 전압을 변경하는 함수
  * @param  channel: 변경할 채널 (0=A, 1=B, ...)
  * @param  voltage: 설정할 전압 값 (단위: Volts)
  * @retval bool: 성공 시 true, 실패 시 false
  */
bool PMIC_Set_Voltage(uint8_t channel, float voltage)
{
    // 1. 목표 전압(아날로그)을 10비트 디지털 값으로 변환
    // 데이터시트 공식: Vout = 0.3V + (digital_value * 2mV)
    if (voltage < 0.3f || voltage > 2.048f) {
        printf("Error: Voltage out of range (0.3V ~ 2.048V).\r\n");
        return false; // 설정 범위 초과
    }
    uint16_t digital_value = (uint16_t)((voltage - 0.3f) / 0.002f);

    // 2. 10비트 값을 상위 2비트(high_byte)와 하위 8비트(low_byte)로 분리
    uint8_t high_byte_val = (digital_value >> 8) & 0x03; // 상위 2비트 추출
    uint8_t low_byte_val = digital_value & 0xFF;        // 하위 8비트 추출

    // 3. 레지스터 주소 설정 (현재는 Buck A 채널만 예시로 구현)
    uint8_t reg_addr_high, reg_addr_low;
    if (channel == 0) { // Buck A 채널
        reg_addr_high = PMIC_REG_VOUTA_VREF_HIGH; // 0x13
        reg_addr_low = PMIC_REG_VOUTA_VREF_LOW;   // 0x14
    } else {
        printf("Error: Channel %d is not supported yet.\r\n", channel);
        return false;
    }

    // 4. I2C 통신으로 레지스터 값 쓰기 (Polling 방식)
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Write(&hi2c1, PMIC_SLAVE_ADDRESS, reg_addr_high, I2C_MEMADD_SIZE_8BIT, &high_byte_val, 1, HAL_MAX_DELAY);
    if (status != HAL_OK)
    {
        printf("Error: Failed to write VREF_HIGH.\r\n");
        return false;
    }

    status = HAL_I2C_Mem_Write(&hi2c1, PMIC_SLAVE_ADDRESS, reg_addr_low, I2C_MEMADD_SIZE_8BIT, &low_byte_val, 1, HAL_MAX_DELAY);
    if (status != HAL_OK)
    {
        printf("Error: Failed to write VREF_LOW.\r\n");
        return false;
    }

    printf("Success: Channel %d voltage set to %.3fV.\r\n", channel, voltage);
    return true;
}