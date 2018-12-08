/**
 *******************************************************************************
 * @file        lora_downlink_shim.cpp
 * @brief       Downlink C-to-C++ shim
 * @author      SIL FW Team
 * @version     v1.0.0
 * @copyright   Swiss Innovation Lab 2018
 *******************************************************************************
 */

/* Includes ----------------------------------------------------------------- */
/* standard */

/* sdk */

/* dependencies */
#ifdef _command_parser_server_hpp
#include "command_parser_server.hpp"
#endif

/* self */
#include "shim.h"

/* Private typedef ---------------------------------------------------------- */

/* Private defines ---------------------------------------------------------- */

/* Private macros ----------------------------------------------------------- */

/* Private variables -------------------------------------------------------- */

/* Private functions -------------------------------------------------------- */

/* Public functions --------------------------------------------------------- */
void Shim_CommandParserWriteReset(uint16_t connHandle)
{
    #ifdef _command_parser_server_hpp
    CommandParserServer::getInstance()->writeReset(connHandle);
    #endif
}

void Shim_CommandParserWriteCommand(uint16_t connHandle, uint8_t commandGroup,
                                    uint8_t commandAction, uint8_t *pCommandParams,
                                    uint8_t commandParamsSize)
{
    #ifdef _command_parser_server_hpp
    CommandParserServer::getInstance()->writeCommand(connHandle, commandGroup,
        commandAction, pCommandParams, commandParamsSize);
    #endif
}

void Shim_CommandParserWriteStream(uint16_t connHandle, uint8_t *pStream,
                                   uint8_t len)
{
    #ifdef _command_parser_server_hpp
    CommandParserServer::getInstance()->writeStream(connHandle, pStream, len);
    #endif
}
