# firmware_nrf52_lib_lora_downlink
LoRa downlink library with integration example

## Prerequisites:

* `#define DEVICE_TYPE` in `project_config.h`
    * will not compile, needed for forwarding requests to config app
* `command_parser_server.c/.h` in `Makefile`
    * required if `lora_downlink` is not set in demo mode
    * if not in demo mode, downlink will not forward data anywhere at all

## Inclusion

Add submodule with parameters:

* URL `https://github.com/swissinnovationlab/firmware_nrf52_lib_lora_downlink.git`
* Relative path `submodules/firmware_nrf52_lib_lora_downlink`
* Source branch `master`

Append `pca10040/s132/armgcc/Makefile`:

    SRC_FILES += \
      ...
      $(SUBMODULES)/firmware_nrf52_lib_lora_downlink/src/lora_downlink.c \
      $(SUBMODULES)/firmware_nrf52_lib_lora_downlink/src/lora_downlink_shim.cpp \
      ...

    INC_FOLDERS += \
      ...
      $(SUBMODULES)/firmware_nrf52_lib_lora_downlink/src \
      ...

Append `project_config.h`

    #define FILE_NAME_LORA_DOWNLINK        BG TBC "loraDownlink"           RST

Include `lora_downlink.h` wherever initialization will be done.

## Configuration



## Usage