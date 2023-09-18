#!/bin/zsh
# Requires mtools
cd "$(dirname $(realpath $0))/.."

PATH="$PWD/tools/bin:$PATH"

mkfat(){
	cd build
	dd if=/dev/zero of=fat.img bs=1k count=1440
	mformat -i fat.img -f 1440 ::
	mmd -i fat.img ::/EFI
	mmd -i fat.img ::/EFI/BOOT
	mcopy -i fat.img BOOTX64.EFI ::/EFI/BOOT
	cd ..
}

mkfat
mkgpt -o build/hd.img \
	--image-size 4096 \
	--part build/fat.img --type system \
	--uuid 746a4f53-746a-6361-756c-1337deadbeef
rm build/fat.img
