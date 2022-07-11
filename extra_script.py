import os
from datetime import datetime

Import("env", "projenv")

# Copy firmware.hex to root dir
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.hex",
    env.VerboseAction(" ".join([
        "$OBJCOPY", 
        f'"{os.path.join("$BUILD_DIR", "${PROGNAME}.hex")}"',
        f'"{os.path.join("$PROJECT_DIR", "f430-1.1.6.hex")}"',
    ]), f"[{datetime.now().strftime('%H:%M:%S')}] Copying ${{PROGNAME}}.hex to f430-1.1.6.hex")
)