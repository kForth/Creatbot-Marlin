import os

Import("env", "projenv")

# Copy firmware.hex to root dir
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.hex",
    env.VerboseAction(" ".join([
        "$OBJCOPY", 
        f'"{os.path.join("$BUILD_DIR", "${PROGNAME}.hex")}"',
        f'"{os.path.join("$PROJECT_DIR", "f430-1.1.6-mod.hex")}"',
    ]), "Copying ${PROGNAME}.hex to f430-1.1.6-mod.hex")
)