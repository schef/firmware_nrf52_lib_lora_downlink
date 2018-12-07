/**
 *******************************************************************************
 * @file        lora_downlink.h
 * @brief       LoRa downlink handler
 * @author      SIL FW Team
 * @version     v1.0.0
 * @copyright   Swiss Innovation Lab 2018
 *******************************************************************************
 */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __LORA_DOWNLINK_H_
#define __LORA_DOWNLINK_H_

/* Define to allow proper C++ linkage --------------------------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
/* standard */
#include <stdint.h>
#include <stdbool.h>

/* Exported types ----------------------------------------------------------- */

/* Public functions --------------------------------------------------------- */
/**
  * @brief  Core downlink handler
  *         The required input format is:
  *         First packet:
  *             {sequenceId commandAction [data0 data1...]}
  *         Subsequent packets:
  *             {sequenceId [data0 data1...]}
  *         sequenceId value 0 resets the command parser
  *         if sequencing is strict, subsequent packets must have sequential IDs
  *         All data bytes are forwarded into the stream, no params
  * @param  standard LoRa_Received_Callback set
  * @retval None
  */
void LoraDownlink_Handler(uint8_t *pData, uint8_t port, uint8_t length);

/**
  * @brief  Initializes downlink module
  *         Registers callback to LoRa module and sets actual functions
  * @param  bIdxRule: force strict write index sequencing
  * @param  bTestMode: use internal mock methods
  * @retval None
  */
void LoraDownlink_Init(bool bIdxRule, bool bTestMode);

/* Define to allow proper C++ linkage --------------------------------------- */
#ifdef __cplusplus
}
#endif

/* Define to prevent recursive inclusion ------------------------------------ */
#endif /* __LORA_DOWNLINK_H_ */
