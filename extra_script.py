import os
from datetime import datetime

OUT_FILE = "f430-1.1.6.hex"

Import("env", "projenv")

# Copy firmware.hex to root dir
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "ihex", "-R", ".eeprom",
        f'"{os.path.join("$BUILD_DIR", "${PROGNAME}.elf")}"',
        f'"{os.path.join("$PROJECT_DIR", OUT_FILE)}"',
    ]), f"Building {OUT_FILE}")
)
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "ihex", "-R", ".eeprom", 
        '"%s"' % os.path.join("$BUILD_DIR", "${PROGNAME}.elf"),
        '"%s"' % os.path.join("\\\\LIGHTPIPE\\F430 Firmware Files", OUT_FILE),
    ]), f"Building {OUT_FILE} on LIGHTPIPE PC")
)