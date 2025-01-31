#ifndef _INCLUDE_CRC_H_
#define _INCLUDE_CRC_H_

#include <stdint.h>
#include <stdbool.h>
#include "hardware.h"

#if WITHFT8

// Compute 14-bit CRC for a sequence of given number of bits
// [IN] message  - byte sequence (MSB first)
// [IN] num_bits - number of bits in the sequence
uint16_t ft8_crc(const uint8_t message[], int num_bits);

/// Extract the FT8 CRC of a packed message (during decoding)
/// @param[in] a91 77 bits of payload data + CRC
/// @return Extracted CRC
uint16_t extract_crc(const uint8_t a91[]);

/// Add the FT8 CRC to a packed message (during encoding)
/// @param[in] payload 77 bits of payload data
/// @param[out] a91 91 bits of payload data + CRC
void add_crc(const uint8_t payload[], uint8_t a91[]);

#endif /* WITHFT8 */
#endif // _INCLUDE_CRC_H_
