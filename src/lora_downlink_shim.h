/**
 *******************************************************************************
 * @file        lora_downlink_shim.h
 * @brief       Downlink C-to-C++ shim
 * @author      SIL FW Team
 * @version     v1.0.0
 * @copyright   Swiss Innovation Lab 2018
 *******************************************************************************
 */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __LORA_DOWNLINK_SHIM_H__
#define __LORA_DOWNLINK_SHIM_H__

/* Define to allow proper C++ linkage --------------------------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
/* standard */
#include <stdint.h>

/* Public functions --------------------------------------------------------- */
void Shim_CommandParserWriteReset(uint16_t connHandle);

void Shim_CommandParserWriteCommand(uint16_t connHandle, uint8_t commandGroup,
                                    uint8_t commandAction, uint8_t *pCommandParams,
                                    uint8_t commandParamsSize);

void Shim_CommandParserWriteStream(uint16_t connHandle, uint8_t *pStream,
                                   uint8_t len);

#ifdef __cplusplus
}
#endif

#endif /* __LORA_DOWNLINK_SHIM_H__ */
