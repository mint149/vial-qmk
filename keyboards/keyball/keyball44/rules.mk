# Link Time Optimization required for size.
LTO_ENABLE = yes

# Build Options
BOOTMAGIC_ENABLE = no       # Enable Bootmagic Lite
EXTRAKEY_ENABLE = no        # Audio control and System control
COMMAND_ENABLE = no         # Commands for debug and configuration
NKRO_ENABLE = no            # Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
AUDIO_ENABLE = no           # Audio output

# This is unnecessary for processing KC_MS_BTN*.
MOUSEKEY_ENABLE = no

# Enabled only one of RGBLIGHT and RGB_MATRIX if necessary.
RGBLIGHT_ENABLE = no        # Enable RGBLIGHT
RGB_MATRIX_ENABLE = no      # Enable RGB_MATRIX (not work yet)
RGB_MATRIX_DRIVER = ws2812

# Do not enable SLEEP_LED_ENABLE. it uses the same timer as BACKLIGHT_ENABLE
SLEEP_LED_ENABLE = no       # Breathing sleep LED during USB suspend

# To support OLED
OLED_ENABLE = no                # Please Enable this in each keymaps.
SRC += lib/oledkit/oledkit.c    # OLED utility for Keyball series.

# Include common library
SRC += lib/keyball/keyball.c

# Disable other features to squeeze firmware size
SPACE_CADET_ENABLE = no
MAGIC_ENABLE = no

# BLE Micro Pro
MCU = cortex-m4
BOOTLOADER = custom
MCU_LDSCRIPT = nrf52840_bmp
CUSTOM_MATRIX = yes # This flag should be on for nrf52
PLATFORM_KEY = bmp
EEPROM_DRIVER = custom
SPLIT_KEYBOARD = no
DYNAMIC_KEYMAP_ENABLE = yes
RAW_ENABLE = yes
MOUSE_SHARED_EP = no
CONSOLE_ENABLE = yes # Console for debug
SRC += lib/bmp/keyboard.c # Support bmp keyboard 
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = custom
SRC += drivers/pmw3360/bmp_pmw3360.c
