#!/bin/zsh
cd "$(dirname $(realpath $0))/.."

command="qemu-system-x86_64 \
	-m 256 \
	-nic user \
	-drive if=pflash,index=0,format=raw,readonly=on,file=qemu/OVMF_CODE-pure-efi.fd \
	-drive if=pflash,index=1,format=raw,file=qemu/OVMF_VARS-pure-efi.fd \
	-accel hvf -cpu qemu64"

if [ "$1" = shell ]; then
	command="$command \
		-drive media=cdrom,format=raw,index=2,file=qemu/UefiShell.iso"
else
	command="$command \
		-drive media=disk,format=raw,index=0,file=build/hd.img"
fi

eval "$command"
