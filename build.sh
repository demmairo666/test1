#!/bin/bash
set -e
echo "Building via docker ps3dev/ps3dev..."
docker run --rm -v "$(pwd):/src" -w /src ps3dev/ps3dev make pkg
ls -lh stalker_demo.pkg
echo "Done. Copy to USB FAT32 and install on PS3 via Install Package Files"
