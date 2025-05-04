#include <stm32f756xx.h>
#include "main.h"
#include "serials.h"
#include "crc.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>


extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;




/**
 * @brief Callback function for I2C2 slave receive completion
 *
 * Handles received data from I2C master, prints debug information,
 * and sets a completion flag. Triggered when a receive operation
 * on I2C2 is complete.
 *
 * @param hi2c Pointer to I2C handle that triggered the callback
 */
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C2) // only if I2C2 is the instance that triggered the callback.
    {
        // Update to print the full received data length
        printf("Data received from I2C master\r\n");

        // Print the last few bytes including CRC for debugging
        printf("Last bytes including CRC: ");
        uint16_t total_len = get_uart_buffer_len(data_rsv1, I2C_BUFFER_SIZE);

        // print only the last 5 bytes including CRC to reduce IRQ overhead.
        crc16_print_buffer((data_rsv1 + total_len - 5), 5);

        i2c2_flag_h = 1;

    }
}

/**
 * @brief Callback function for UART2 receive completion
 *
 * Handles received data from UART2, sets a completion flag,
 * and re-enables interrupt-driven receive for the next data transfer.
 *
 * @param huart Pointer to UART handle that triggered the callback
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        uart2_flag_calback = 1;
        HAL_UART_Receive_IT(&huart2, uart_buffer, 10);                          // only more then 10 bytes will trigger IRQ, or timeout.
    }
}

// printf implementation for UART3.
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

// printf implementation for UART3.
int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart3, (uint8_t *)ptr, len, 0xFFFF);
    return len;
}

// scanf implementation for UART3. (not used in this code)
int _read(int file, char *ptr, int len)
{
    int ch = 0;
    HAL_UART_Receive(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    if (ch == 13)
    {
        ch = 10;
        HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    }
    else if (ch == 8)
    {
        ch = 0x30;
        HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    }

    *ptr = ch;

    return 1;
}

/**
 * @brief Determines the length of a UART buffer by finding the first occurrence of a newline or null terminator
 *
 * @param buffer Pointer to the input buffer to measure
 * @param len Maximum length to search in the buffer
 * @return uint16_t Length of the buffer up to the first newline or null terminator
 */
uint16_t get_uart_buffer_len(const uint8_t *buffer, uint16_t len)
{
    uint16_t len_buffer = 0;
    for (uint16_t i = 0; i < len; i++)
    {
        if ((buffer[i] == '\r' && buffer[i + 1] == '\n') || (buffer[i] == 0))
        {
            len_buffer = i;
            break;
        }
    }
    return len_buffer;
}

/**
 * @brief This function checks the UART buffer and sends the data to I2C if the CRC is correct
 * Note: not getting for now the CRC from the UART buffer, it get standard data without CRC.
 *
 * @param buffer received from UART
 * @param crc_buffer crc buffer
 * @param len
 * @return uint8_t (HAL_Status) 0 if CRC is correct, 1 if
 */
uint8_t check_uart_buffer(uint8_t *uart_buffer, uint8_t *uart2_flag_calback, uint16_t len)
{
    HAL_StatusTypeDef status;
    uint8_t retval = STATUS_NO_DATA;                                            // return only if not data in buffer
    uint8_t txBuf[UART_BUFFER_SIZE];
    if (*uart2_flag_calback)
    {
        uint16_t len_buffer = get_uart_buffer_len(uart_buffer, len);
        if (len_buffer > 2)
        {
            memcpy(txBuf, uart_buffer, len_buffer);
            uint16_t tx_crc = crc16(txBuf, len_buffer);
            crc16_update_buffer(tx_crc, txBuf, len_buffer);
            printf("[check_uart_buffer] CRC: %02X\r\n", tx_crc);
            crc16_print_buffer(txBuf, len_buffer);
            memset(data_rsv1, 0, I2C_BUFFER_SIZE + 2);                          // Clear the buffer + 2 bytes for CRC
            HAL_I2C_Slave_Receive_IT(&hi2c2, data_rsv1, len_buffer + 2);

            status = HAL_I2C_Master_Transmit(&hi2c1, 68, txBuf, len_buffer + 2, 1000);
            if (status == HAL_OK)
            {
                printf("[check_uart_buffer] HAL_OK: Success sending data over I2C. Status: %d\r\n", status);
                // Re-enable with the correct buffer size
            }
            else if (status == HAL_ERROR)
                printf("[check_uart_buffer] HAL_ERROR: Check for bus errors\r\n");
            else if (status == HAL_BUSY)
                printf("[check_uart_buffer] HAL_BUSY: I2C bus or peripheral is busy\r\n");
            else if (status == HAL_TIMEOUT)
                printf("[check_uart_buffer] HAL_TIMEOUT: Timeout waiting for I2C operation\r\n");
            retval = status;
        }
        else
        {
            printf("[check_uart_buffer] No data to send\r\n");
            retval = STATUS_ERROR;
        }
        *uart2_flag_calback = 0;
        memset(uart_buffer, 0, UART_BUFFER_SIZE + 1);
        HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
        printf("[check_uart_buffer] I2C data: %s\r\n", (char *)txBuf);
        }
    return retval;
}

/**
 * @brief Checks and processes I2C2 buffer data with CRC validation
 * 
 * @param i2c_buffer Pointer to the I2C receive buffer
 * @param i2c2_flag_h Flag indicating I2C2 buffer is ready for processing
 * @param len Total length of the received buffer
 * 
 * @return uint8_t 0 if CRC validation passes, 1 if CRC validation fails
 */
uint8_t check_i2c2_buffer(uint8_t *i2c_buffer, uint8_t *i2c2_flag_h, uint16_t len)
{
    uint8_t retval = STATUS_NO_DATA;
    if (*i2c2_flag_h)
    {
        uint16_t len_buffer = get_uart_buffer_len(i2c_buffer, len);
        HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 1000);
        HAL_UART_Transmit(&huart2, i2c_buffer, len_buffer - 2, 1000);           // withot CRC
        HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, 1000);
        crc16_print_buffer(i2c_buffer, len_buffer);                             // complete I2C buffer
        uint16_t rx_crc = crc16(i2c_buffer, len_buffer - 2);                    // calculate CRC by data only
        uint16_t crc_recived = crc16_extract_crc(data_rsv1, len_buffer);        // extract CRC from received data
        
        // compare calculated CRC from data with CRC from received data (last 2 bytes)
        if (crc_recived == rx_crc)                                              
        {
            printf("[check_i2c2_buffer] CRC recived: %02X\r\n", crc_recived);
            printf("[check_i2c2_buffer] The crc is %s\r\n", crc_recived == rx_crc ? "OK" : "NOK");
            printf("[check_i2c2_buffer] I2C data: #%d\r\n", i++);
            crc16_print_buffer(i2c_buffer, len_buffer);
            memset(i2c_buffer, 0, I2C_BUFFER_SIZE + 1);
            retval = STATUS_OK;
        }
        else                                                                    // CRC is not correct
        {
            printf("[check_i2c2_buffer] CRC NOK: %d\r\n", rx_crc);
            printf("[check_i2c2_buffer] The crc is %s\r\n", crc_recived == rx_crc ? "OK" : "NOK");
            printf("[check_i2c2_buffer] I2C data: #%d\r\n", i);
            crc16_print_buffer(i2c_buffer, len_buffer);
            memset(i2c_buffer, 0, I2C_BUFFER_SIZE + 1);
            retval = STATUS_ERROR;                                              // return error status (1)
        }
        *i2c2_flag_h = 0;
    }
    return retval;
}
