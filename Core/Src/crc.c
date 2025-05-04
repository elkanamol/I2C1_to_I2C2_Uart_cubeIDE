#include "crc.h"
#include <stdio.h>

#define CRC16_CCITT_INIT_VALUE 0xFFFF
#define CRC16_CCITT_POLYNOMIAL 0x1021
#define CRC16_CCITT_XOR_VALUE 0xFFFF

/**
 * @brief Calculates the 16-bit CRC (Cyclic Redundancy Check) using CCITT standard.
 *
 * This function computes the CRC-16-CCITT checksum for the given data buffer
 * using the CCITT polynomial (0x1021) with an initial value of 0xFFFF and
 * final XOR value of 0xFFFF.
 *
 * @param data Pointer to the input data buffer
 * @param length Number of bytes in the data buffer
 * @return uint16_t The calculated 16-bit CRC value
 */
uint16_t crc16(const uint8_t *data, uint16_t length)
{
    uint16_t crc = CRC16_CCITT_INIT_VALUE;                  // start CRC with 0xFFFF
    
    for (uint16_t i = 0; i < length; i++)
    {
        crc ^= (uint16_t)data[i] << 8;                      // XOR with the current byte, shifted left by 8 bits
        for(uint8_t bit = 0; bit < 8; bit++)
        {
            if (crc & 0x8000)                               // if the most significant bit is set
            {
                crc = (crc << 1) ^ CRC16_CCITT_POLYNOMIAL;  // shift left and XOR with the polynomial
            }
            else
            {
                crc <<= 1;                                  // only shift left without XOR
            }
        }
    }
    return crc ^ CRC16_CCITT_XOR_VALUE;
}

/**
 * @brief Updates a buffer with the 16-bit CRC value at the specified length.
 *
 * This function writes the most significant and least significant bytes
 * of the provided CRC value into the data buffer at the specified length
 * and length + 1 indices. e.g passing 0x1234 as crc and 2 as length will
 * write 0x12 to data[2] and 0x34 to data[3].
 * 
 *
 * @param crc The 16-bit CRC value to be written to the buffer
 * @param data Pointer to the buffer to be updated
 * @param length Index where the CRC bytes will be written
 */
void crc16_update_buffer(uint16_t crc, uint8_t *data, uint16_t length)
{
    data[length] = (uint8_t)(crc  >> 8);        // shift right by 8 bits and store in the buffer
    data[length + 1] = (uint8_t)(crc & 0xFF);   // store the least significant byte in the buffer
}


/**
 * @brief Extracts the 16-bit CRC value from a data buffer.
 *
 * This function retrieves the CRC value from the last two bytes of the provided
 * data buffer, assuming little-endian CRC storage. If the buffer is too short
 * to contain a CRC, it returns 0.
 *
 * @param data Pointer to the buffer containing the CRC
 * @param length Total length of the buffer
 * @return uint16_t The extracted 16-bit CRC value, or 0 if insufficient data
 */
uint16_t crc16_extract_crc(uint8_t *data, uint16_t length)
{
    if (length < 2)
    {
        return 0;
    }

    // For little-endian CRC storage
    return (uint16_t)(data[length - 2] << 8) | data[length - 1];
}

/**
 * @brief Prints the contents of a buffer in hexadecimal format.
 *
 * This function iterates through the given buffer and prints each byte
 * as a two-digit hexadecimal value, followed by a space. A newline is
 * printed after all bytes have been displayed.
 *
 * @param data Pointer to the buffer to be printed
 * @param length Number of bytes in the buffer to print
 */
void crc16_print_buffer(uint8_t *data, uint16_t length)
{
    printf("Buffer: ");
    for (uint16_t i = 0; i < length; i++)
    {
        printf("%02X ", data[i]);
    }
    printf("\n");
}


// uint16_t crc16_extract_crc(uint8_t *data, uint16_t length)
// {
//     // Assuming the CRC is the last 2 bytes of the data
//     // And length is the total length including CRC
//     if (length < 2)
//     {
//         return 0; // Not enough data to contain a CRC
//     }

//     // Correct indexing for the last two bytes
//     return (uint16_t)(data[length - 2] << 8) | data[length - 1];
// }