/**
 *******************************************************************************
 * @file        lora_downlink.h
 * @brief       LoRa downlink handler
 * @author      SIL FW Team
 * @version     v1.0.0
 * @copyright   Swiss Innovation Lab 2018
 *******************************************************************************
 */

/* Includes ----------------------------------------------------------------- */
/* standard */

/* sdk */

/* dependencies */
#include "lora.h"
#include "lora_downlink_shim.h"

/* logger */
#define FILE_NAME FILE_NAME_LORA_DOWNLINK
#include "log.h"

/* self */
#include "lora_downlink.h"

/* Private typedef ---------------------------------------------------------- */
/**
  * @brief Collection of function pointers needeed for operation
  */
typedef struct _LoraDownlink_FnPtrs
{
    void (*reset)(uint16_t);
    void (*command)(uint16_t, uint8_t, uint8_t, uint8_t *, uint8_t);
    void (*stream)(uint16_t, uint8_t *, uint8_t);
} LoraDownlink_FnPtrs;

/* Private defines ---------------------------------------------------------- */
/**
  * @brief debug-only defined static - prevents unit testing in production
  */
#ifdef DEBUG
#define STATIC
#else
#define STATIC static
#endif

/**
  * @brief Write index indicating a command parser reset is needed
  */
#define WRITE_INDEX_RESET   0x00

/**
  * @brief Command group is set to the device type by default
  */
#define COMMAND_GROUP       DEVICE_TYPE

/**
  * @brief Allows treating ConnHandle with value NRF_SDH_BLE_TOTAL_LINK_COUNT
  *        as special "bypass" channel from LoRa downlink
  */
#ifdef COMMAND_PARSER_MAX_CONN_HANDLES
#undef COMMAND_PARSER_MAX_CONN_HANDLES
#endif
#define COMMAND_PARSER_MAX_CONN_HANDLES (NRF_SDH_BLE_TOTAL_LINK_COUNT + 1)

/**
  * @brief Default connection handle matches the total BLE link country
  *        This way the downlink handle doesn't match an actual BLE link handle
  */
#define CONN_HANDLE         NRF_SDH_BLE_TOTAL_LINK_COUNT

/* Private variables -------------------------------------------------------- */
LoraDownlink_FnPtrs FnPtrs;
bool bIndexRuleStrict = false;
bool bInitialized = false;

/* Private functions -------------------------------------------------------- */
/**
  * @brief Mockup write reset
  */
static void writeReset(uint16_t conn_handle)
{
    Log_printInfo(FILE_NAME, "doReset handle %d\r\n", conn_handle);
}

/**
  * @brief Mockup write command
  */
static void writeCommand(uint16_t conn_handle,
                                       uint8_t commandGroup,
                                       uint8_t commandAction,
                                       uint8_t *commandParams,
                                       uint8_t commandParamsSize)
{
    Log_printInfo(FILE_NAME, "writeCommand %02X:%02X, connHandle %d\r\n", commandGroup, commandAction, conn_handle);
}

/**
  * @brief Mockup write stream
  */
static void writeStream(uint16_t conn_handle, uint8_t *pStream, uint8_t len)
{
    Log_printInfo(FILE_NAME, "writeStream: ");
    Log_printBytes(pStream, len);
}

/**
  * @brief  Core downlink handler
  *         The required input format is:
  *         First packet:
  *             {sequenceId commandAction [data0 data1...]}
  *         Subsequent packets:
  *             {sequenceId [data0 data1...]}
  *         sequenceId value 0 resets the command parser
  *
  *         if sequencing is strict, subsequent packets must have sequential IDs
  *         All data bytes are forwarded into the stream, no params
  * @param  standard LoRa_Received_Callback set
  * @retval None
  */
STATIC void LoraDownlink_Handler(uint8_t *pData, uint8_t port, uint8_t length)
{
    if (!bInitialized)
    {
        return;
    }

    uint8_t offset = 0;
    static uint8_t writeIndex = WRITE_INDEX_RESET;

    Log_printInfo(FILE_NAME, "Downlink received from port %d\r\n", port);

    if (length < 2)
    {
        return;
    }

    if (pData[0] == WRITE_INDEX_RESET)
    {
        FnPtrs.reset(CONN_HANDLE);
        FnPtrs.command(CONN_HANDLE, COMMAND_GROUP, pData[1], NULL, 0);
        writeIndex = 1;
        offset = 2;
    }
    else
    {
        if (bIndexRuleStrict && pData[0] != writeIndex)
        {
            Log_printInfo(FILE_NAME, "OutOfSequenceError\r\n");
            writeIndex = WRITE_INDEX_RESET;
            return;
        }
        writeIndex = writeIndex + 1;
        offset = 1;
    }

    FnPtrs.stream(CONN_HANDLE, &pData[offset], length-offset);
}

/* Public functions --------------------------------------------------------- */
/**
  * @brief  Initializes downlink module
  *         Registers callback to LoRa module and sets actual functions
  * @param  bIdxRule: force strict write index sequencing
  * @param  bTestMode: use internal mock methods
  * @retval None
  */
void LoraDownlink_Init(bool bIdxRule, bool bTestMode)
{
    bIndexRuleStrict = bIdxRule;

    if (bTestMode)
    {
        FnPtrs.reset = writeReset;
        FnPtrs.command = writeCommand;
        FnPtrs.stream = writeStream;
    }
    else
    {
        FnPtrs.reset = Shim_CommandParserWriteReset;
        FnPtrs.command = Shim_CommandParserWriteCommand;
        FnPtrs.stream = Shim_CommandParserWriteStream;
    }

    LoRa_Received_registerCallback(LoraDownlink_Handler);

    Log_printInfo(FILE_NAME, "Init: strict %c test %c\r\n",
        bIdxRule ? 'y':'n', bTestMode ? 'y':'n');

    bInitialized = true;
}
