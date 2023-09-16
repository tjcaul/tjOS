#!/bin/zsh
cd "$(dirname $(realpath $0))/.."

scripts/mount.sh
cp build/BOOTX64.EFI /Volumes/No\ Name/EFI/BOOT/
scripts/umount.sh
