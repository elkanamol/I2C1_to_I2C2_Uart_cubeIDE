#ifndef SERIALS_H
#define SERIALS_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "stm32f7xx_hal.h"

#define STATUS_OK 0
#define STATUS_ERROR 1
#define STATUS_BUSY 2
#define STATUS_TIMEOUT 3
#define STATUS_NO_DATA 4

    uint16_t get_uart_buffer_len(const uint8_t *buffer, uint16_t len);
    uint8_t check_i2c2_buffer(uint8_t *i2c_buffer, uint8_t *i2c2_flag_h, uint16_t len);
    uint8_t check_uart_buffer(uint8_t *uart_buffer, uint8_t *uart2_flag_calback, uint16_t len);

    // replace the HAL waek functions by own implemetation
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
    void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);

    int __io_putchar(int ch);
    int _write(int file, char *ptr, int len);
    int _read(int file, char *ptr, int len);

#ifdef __cplusplus
}
#endif

#endif