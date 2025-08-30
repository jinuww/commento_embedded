#include "eeprom_25lc256.h"

static inline void CS_L(GPIO_TypeDef* p, uint16_t pin){ HAL_GPIO_WritePin(p, pin, GPIO_PIN_RESET); }
static inline void CS_H(GPIO_TypeDef* p, uint16_t pin){ HAL_GPIO_WritePin(p, pin, GPIO_PIN_SET); }

void EEPROM25_WriteEnable(SPI_HandleTypeDef* hspi, GPIO_TypeDef* port, uint16_t pin){
  uint8_t cmd = EEPROM25_CMD_WREN;
  CS_L(port,pin); HAL_SPI_Transmit(hspi, &cmd, 1, HAL_MAX_DELAY); CS_H(port,pin);
}

HAL_StatusTypeDef EEPROM25_Read(SPI_HandleTypeDef* hspi, GPIO_TypeDef* port, uint16_t pin,
                                uint16_t addr, uint8_t* buf, size_t len){
  uint8_t cmd[3] = { EEPROM25_CMD_READ, (uint8_t)(addr>>8), (uint8_t)addr };
  CS_L(port,pin);
  HAL_SPI_Transmit(hspi, cmd, 3, HAL_MAX_DELAY);
  HAL_StatusTypeDef st = HAL_SPI_Receive(hspi, buf, len, HAL_MAX_DELAY);
  CS_H(port,pin);
  return st;
}

HAL_StatusTypeDef EEPROM25_Write(SPI_HandleTypeDef* hspi, GPIO_TypeDef* port, uint16_t pin,
                                 uint16_t addr, const uint8_t* buf, size_t len){
  size_t off = 0;
  while (off < len){
    size_t page_rem = EEPROM25_PAGE_SIZE - ((addr + off) % EEPROM25_PAGE_SIZE);
    size_t chunk = (len - off < page_rem) ? (len - off) : page_rem;
    uint8_t cmd[3] = { EEPROM25_CMD_WRITE, (uint8_t)((addr+off)>>8), (uint8_t)(addr+off) };
    EEPROM25_WriteEnable(hspi, port, pin);
    CS_L(port,pin);
    HAL_SPI_Transmit(hspi, cmd, 3, HAL_MAX_DELAY);
    HAL_StatusTypeDef st = HAL_SPI_Transmit(hspi, (uint8_t*)&buf[off], chunk, HAL_MAX_DELAY);
    CS_H(port,pin);
    if (st != HAL_OK) return st;
    HAL_Delay(5); // 내부 write cycle
    off += chunk;
  }
  return HAL_OK;
}
