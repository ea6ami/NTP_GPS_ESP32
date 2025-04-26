import os
import re

Import("env")

# Leer version.h
version_file = os.path.join(env['PROJECT_DIR'], "include", "version.h")
with open(version_file, "r") as f:
    content = f.read()

# Buscar la versión como string
match = re.search(r'#define\s+FW_VERSION_STRING\s+"([\d\.]+)"', content)
if match:
    version = match.group(1)
else:
    version = "0.0.0"

# Nombre final del firmware
firmware_name = f"firmware-v{version}.bin"

# Establecer nombre final
env.Replace(PROGNAME=firmware_name)
