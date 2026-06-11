/**
*   @file alg_crc.h
*   @brief 
*   @author Wenxin HU
*   @date 25-7-13
*   @version 0.5
*   @note
*
*/
#ifndef ALG_CRC_H
#define ALG_CRC_H

#include <stdint.h>
#include <stdlib.h>

/**
 * @brief 用于计算缓冲区内数据的CRC-CCITT校验和
 * @param crc CRC初始值
 * @param buffer 缓冲区指针
 * @param len 缓冲区长度
 * @return CRC校验和
 */
uint16_t Crc_Ccitt_Calculate(uint16_t crc, uint8_t const *buffer, size_t len);

/**
 *
 * @param Data 传入待校验数据(不含校验位）CRC-8 的校验码占 1 字节（length 1）
 * @param Lenth 传入数据长度(不含校验位）
 * @return 计算出的CRC-8校验码
 */
uint8_t CRC08_Calculate(uint8_t  *Data, uint32_t Lenth /* Without check code lenth 1 */);

/**
 *
 * @param Data 传入待校验数据(含校验位）CRC-16 的校验码占 1 字节（length 1）)
 * @param Lenth 传入数据长度(含校验位）
 * @return 是否验证成功
 */
uint32_t CRC08_Verify(uint8_t *Data, uint32_t Lenth /* With check code lenth 1 */);

/**
 *
 * @param Data 传入待校验数据(不含校验位）CRC-16 的校验码占 2 字节（length 2）
 * @param Lenth 传入数据长度(不含校验位）
 * @return 计算出的CRC-16校验码
 */
uint16_t CRC16_Calculate(uint8_t  *Data, uint32_t Lenth /* Without check code lenth 2 */);

/**
 *
 * @param Data 传入待校验数据(含校验位）CRC-16 的校验码占 2 字节（length 2）)
 * @param Lenth 传入数据长度(含校验位）
 * @return 是否验证成功
 */
uint32_t CRC16_Verify(uint8_t *Data, uint32_t Lenth /* With check code lenth 2 */);


#endif //ALG_CRC_H
