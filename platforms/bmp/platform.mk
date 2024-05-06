SRC += $(PLATFORM_COMMON_DIR)/wait.c
SRC += $(PLATFORM_COMMON_DIR)/drivers/eeprom/eeprom_bmp.c

ifeq ($(strip $(SPLIT_KEYBOARD)), yes)
    SRC += $(PLATFORM_COMMON_DIR)/transport.c
endif

CONFIG_H += $(PLATFORM_COMMON_DIR)/config.h
POST_CONFIG_H += $(PLATFORM_COMMON_DIR)/post_config.h

GIT_DESCRIBE = $(shell git describe --tags --long --dirty="\\*")
CFLAGS += -DGIT_DESCRIBE=$(GIT_DESCRIBE)
CFLAGS += -DTARGET=$(TARGET)

UF2_FAMILY = NRF52840
NRFUTIL = nrfutil

# Linker script selection.
##############################################################################

ifneq ("$(wildcard $(KEYBOARD_PATH_5)/ld/$(MCU_LDSCRIPT).ld)","")
    LDSCRIPT = $(KEYBOARD_PATH_5)/ld/$(MCU_LDSCRIPT).ld
else ifneq ("$(wildcard $(KEYBOARD_PATH_4)/ld/$(MCU_LDSCRIPT).ld)","")
    LDSCRIPT = $(KEYBOARD_PATH_4)/ld/$(MCU_LDSCRIPT).ld
else ifneq ("$(wildcard $(KEYBOARD_PATH_3)/ld/$(MCU_LDSCRIPT).ld)","")
    LDSCRIPT = $(KEYBOARD_PATH_3)/ld/$(MCU_LDSCRIPT).ld
else ifneq ("$(wildcard $(KEYBOARD_PATH_2)/ld/$(MCU_LDSCRIPT).ld)","")
    LDSCRIPT = $(KEYBOARD_PATH_2)/ld/$(MCU_LDSCRIPT).ld
else ifneq ("$(wildcard $(KEYBOARD_PATH_1)/ld/$(MCU_LDSCRIPT).ld)","")
    LDSCRIPT = $(KEYBOARD_PATH_1)/ld/$(MCU_LDSCRIPT).ld
endif

# Shared Compiler flags for all toolchains
SHARED_CFLAGS += -mcpu=cortex-m4
SHARED_CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
SHARED_CFLAGS += -mthumb -mabi=aapcs
SHARED_CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
SHARED_CFLAGS += -fno-builtin -fshort-enums

MCUFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant
# Linker flags
LDFLAGS += -mthumb -mabi=aapcs

LDSCRIPT_PATH := $(shell dirname "$(LDSCRIPT)")
LDFLAGS += -L$(LDSCRIPT_PATH) -T$(LDSCRIPT)

LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys
LDFLAGS += -Xlinker --print-memory-usage

# ARM toolchain specific configuration
TOOLCHAIN ?= arm-none-eabi-

# Assembler flags
ASFLAGS  += $(SHARED_ASFLAGS) $(TOOLCHAIN_ASFLAGS)

# C Compiler flags
CFLAGS   += $(SHARED_CFLAGS) $(TOOLCHAIN_CFLAGS)

# C++ Compiler flags
CXXFLAGS += $(CFLAGS) $(SHARED_CXXFLAGS) $(TOOLCHAIN_CXXFLAGS) -fno-rtti

# Linker flags
LDFLAGS  += $(SHARED_LDFLAGS) $(SHARED_LDSYMBOLS) $(TOOLCHAIN_LDFLAGS) $(TOOLCHAIN_LDSYMBOLS) $(MCUFLAGS)

# Tell QMK that we are hosting it on BLE Micro Pro.
OPT_DEFS += -DPROTOCOL_BMP

# Construct GCC toolchain
CC      = $(CC_PREFIX) $(TOOLCHAIN)gcc
OBJCOPY = $(TOOLCHAIN)objcopy
OBJDUMP = $(TOOLCHAIN)objdump
SIZE    = $(TOOLCHAIN)size
AR      = $(TOOLCHAIN)ar
NM      = $(TOOLCHAIN)nm
HEX     = $(OBJCOPY) -O $(FORMAT)
EEP     =
BIN     = $(OBJCOPY) -O binary


zip: $(BUILD_DIR)/$(TARGET).bin
	if ! type "nrfutil" > /dev/null 2>&1; then \
		echo 'ERROR: nrfutil is not found'; exit 1;\
	fi
	$(NRFUTIL) pkg generate --debug-mode --hw-version 52 --sd-req 0xA9 --application $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).zip

nrfutil: zip
	printf "Detecting USB port, put your controller into dfu-mode now."; \
	while [ -z $$USB ]; do \
		sleep 0.5; \
		printf "."; \
		ls /dev/tty* > /tmp/2; \
		USB=`ls /dev/tty.* | grep usbmodem | grep -v vial`; \
	done; \
	echo ""; \
	echo "Detected controller on USB port at $$USB"; \
	sleep 1; \
	echo "Programming Started"; \
	$(NRFUTIL) dfu usb-serial -pkg $(BUILD_DIR)/$(TARGET).zip -p $$USB; \
