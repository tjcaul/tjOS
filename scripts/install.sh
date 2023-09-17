#!/bin/zsh
cd "$(dirname $(realpath $0))/.."

volume="${1:-/Volumes/No Name/EFI/}"
scripts/mount.sh
cp build/BOOTX64.EFI $volume/BOOT/
scripts/umount.sh
