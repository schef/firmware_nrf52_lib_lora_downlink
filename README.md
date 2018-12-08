# firmware_nrf52_lib_lora_downlink v1.0.0
LoRa downlink library with integration example

## Prerequisites:
* `#define DEVICE_TYPE` in `project_config.h`
    * will not compile otherwise, needed for registering Command Parser commands
* `lora.c/.h` in `Makefile`
    * will not compile otherwise, needed for registering LoRa received callback
* `command_parser_server.c/.h` in `Makefile`
    * required if `lora_downlink` is not set in demo mode
    * if not in demo mode, downlink will not forward data anywhere at all

## Inclusion
Add submodule with parameters:

* URL `https://github.com/swissinnovationlab/firmware_nrf52_lib_lora_downlink.git`
* Relative path `submodules/firmware_nrf52_lib_lora_downlink`
* Source branch `master`

Append `./src/pca10040/s132/armgcc/Makefile`:

    SRC_FILES += \
      ...
      $(SUBMODULES)/firmware_nrf52_lib_lora_downlink/src/lora_downlink.c \
      $(SUBMODULES)/firmware_nrf52_lib_lora_downlink/src/lora_downlink_shim.cpp \
      ...

    INC_FOLDERS += \
      ...
      $(SUBMODULES)/firmware_nrf52_lib_lora_downlink/src \
      ...

Append `./src/project_config.h`

    #define FILE_NAME_LORA_DOWNLINK        BG TBC "loraDownlink"           RST

Include `lora_downlink.h` wherever initialization will be done.

## Setup
1. Initialize BLE wrapper
2. Initialize LoRa manager
3. Register only write commands to Command Parser, group `DEVICE_TYPE`, action ID as required
4. Initialize LoRa downlink using `LoraDownlink_Init(bool bIdxRule, bool bTestMode)`
    * `bIdxRule` - if true, requires strict command sequences (for multipart commands)
    * `bTestMode` - if true, only prints received commands instead of forwarding to Command Parser
5. On receiving a command through the `LoraDownlink` module, it will execute it as any other Command Parser command

# Downlink process explained
LoRa downlink data transmission is available in both OTAA and ABP modes.
However, downlinks can only be sent if the client device sent a CNF-type message.
This also requires that the gateway already has a prepared packet it will respond with.
## Downlink process UART traffic
During standard (no downlink) LoRa operation, a successful example LoRa CNF transmission (`mac tx cnf`) will generate the following UART traffic:

    uart tx: mac tx cnf 15 9A5BFE96AD
    uart rx: ok
    uart rx: mac_tx_ok

During downlink LoRa operation, a successful LoRa CNF send will generate the following UART traffic:

    uart tx: mac tx cnf 15 9A5BFE9B8A
    uart rx: ok
    uart rx: mac_rx 15 00005BFD6552

The first argument after the `mac_rx` section is the port, followed by the payload.
The LoRa downlink payload is then parsed and, if the format is correct, forwarded to the function registered using `LoRa_Received_registerCallback`.

## LoRa downlink handler
The LoRa downlink module expects one of two following formats of packets:

* Initial payload `{0x00 commandAction [data0 data1...]}`
* Appended payload `{writeIndex [data0 data1]}`

The appended payload is optional in case the entire message could not fit in one downlink.
Depending on the value of the first byte, the following processing logic is applied:
* if the `writeIndex` (first byte) value is `0x00`, this is an Initial Payload
    * the Command Parser will be reset
    * the Command Parser Write Command will be invoked
        * the command group will be set to the project-set `DEVICE_TYPE`
        * the command action will be set to the received `commandAction`
        * no data is forwarded as part of the Command Parameters
    * the remainder of the payload will be sent to the Command Parser Write Stream
    * the expected write Index will be incremented

* if the `writeIndex` value is non-zero, this is an Appended payload
    * if the strict index flag is not set
        * the payload will be sent to the Command Parser Write Stream
	        * _this may be unsafe to use in production_
        * the expected write Index will be incremented
    * if the strict index flag is set and the `writeIndex` matches the expected write index
        * the payload will be sent to the Command Parser Write Stream
        * the expected write index will be incremented
    * if the strict index flag is set, but the `writeIndex` does not match the expected index
        * the payload will be discarded with a logged error

To demonstrate the behaviour of the strict-index flag: in case there was a registered command `0x9A 0xBF`, with the strict-index flag set, this message sequence is valid:

    0x00 0xBF 0x01 0x23 0x45 0x67 0x89 // initial payload, action 0xBF, stream 0x01...
    0x01 0xAB 0xCD 0xEF                // appended payload, stream 0xAB 0xCD 0xEF
    0x02 0xFE 0xDC 0xBA                // appended payload, stream 0xFE 0xDC 0xBA

The resulting stream would be `0x01 0x23 0x45 0x67 0x89 0xAB 0xCD 0xEF 0xFE 0xDC 0xBA`.
The following message sequence would not be valid:

    0x00 0x00 0x01 0x23 0x45 0x67 0x89 // initial payload, action 0xBF, stream 0x01..
    0x01 0xAB 0xCD 0xEF                // appended payload, stream 0xAB 0xCD 0xEF
    0x03 0xFE 0xDC 0xBA                // mismatched sequence ID, error logged
 The resulting stream would `0x01 0x23 0x45 0x67 0x89 0xAB 0xCD 0xEF`, however, an error would be logged with the last message because the sequence ID is incorrect.

## Distinguishing commands issued through BLE vs. LoRa downlink
In production, when creating the write command handler, the `connHandle` input parameter may be checked to determine the source. If `connHandle == NRF_SDH_BLE_TOTAL_LINK_COUNT` is true, the command was received through the LoRa downlink, otherwise it came from a BLE device.
