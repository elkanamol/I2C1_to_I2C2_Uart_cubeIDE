#ifndef CRC_H
#define CRC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    uint16_t crc16(const uint8_t *data, uint16_t length);

    void crc16_update_buffer(uint16_t crc, uint8_t *data, uint16_t length);

    uint16_t crc16_extract_crc(uint8_t *data, uint16_t length);
    
    void crc16_print_buffer(uint8_t *data, uint16_t length);

    
#ifdef __cplusplus
}
#endif

#endif /* CRC_H */
