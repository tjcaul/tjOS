#!/bin/zsh
cd "$(dirname $(realpath $0))/.."
export PATH="$PWD/tools/bin:$PATH"

gcc-o(){
	mkdir -p "build/$(dirname $1)"
	x86_64-w64-mingw32-gcc \
		-ffreestanding \
		-fpic \
		-fno-stack-protector \
		-fno-stack-check \
		-fshort-wchar \
		-mno-red-zone \
		-maccumulate-outgoing-args \
		-Iinclude \
		-Iinclude/gnu-efi \
		-Iinclude/gnu-efi/x86_64 \
		-Iinclude/gnu-efi/protocol \
		-c -o "build/$1.o" "src/$1.c"
}

gcc(){
	find build -name '*.o' |
	xargs \
	x86_64-w64-mingw32-gcc \
	-shared \
	-nostdlib \
	-Wl,-dll \
	-Wl,--subsystem,10 \
	-Llib \
	-lefi \
	-lgnuefi \
	-e efi_main \
	-o build/BOOTX64.EFI
}

cleanup(){
	rm build/*.o
}

rm -r build/*

find src -name '*.c' |
sed -e 's|^src/||' -e 's/.c$//' |
while read cfile; do
	gcc-o "$cfile"
done

gcc

cleanup
