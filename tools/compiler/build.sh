#!/bin/zsh

entry="${1:-all}"

thisdir="$(dirname $(realpath $0))"
prefix="$thisdir"
target=x86_64-w64-mingw32
PATH="$prefix/bin:$PATH"

big-print(){
	len=$(echo -n "$*" | wc -c | tr -d ' ')
	bar='=='
	for i in $(seq 1 $len); do
		bar="$bar="
	done
	echo ".$bar."
	echo "| $@ |"
	echo "'$bar'"
}

case "$entry" in
all)
	brew install make texinfo zlib zstd libelf
	;&

download)
	big-print DOWNLOADING
	cd $thisdir
	rm -r bin binutils-* build-binutils build-gcc gcc-* include lib libexec share x86_64-*
	(
		wget https://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.xz &
		wget https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.xz &
		wait
	)
	tar -xf binutils-*.tar.xz &
	tar -xf gcc-*.tar.xz &
	wait
	rm *.tar.xz
	;&

binutils)
	big-print COMPILING BINUTILS
	cd $thisdir
	mkdir build-binutils
	cd build-binutils
	../binutils-*/configure --target="$target" --prefix="$prefix" --with-sysroot --disable-nls --disable-werror
	make -j2 all-gas all-ld all-binutils
	make -j2 install-gas install-ld install-binutils
	;&

no-red-zone)
	cd $thisdir/gcc-*
	big-print ENABLING no-red-zone
	cat > gcc/config/i386/t-x86_64-elf <<EOF
MULTILIB_OPTIONS += mno-red-zone
MULTILIB_DIRNAMES += no-red-zone
EOF
	sed -i .bak 's|x86_64-\*-elf\*)|&\ntmake_file="${tmake_file} i386/t-x86_64-elf" # include the new multilib configuration|' gcc/config.gcc
	;&

gcc)
	big-print COMPILING GCC
	cd $thisdir
	mkdir build-gcc
	cd build-gcc
	../gcc-*/configure --target="$target" --prefix="$prefix" --disable-nls --enable-languages=c --without-headers
	make -j2 all-gcc
	make -j2 all-target-libgcc
	make -j2 install-gcc
	make -j2 install-target-libgcc
	;&

ln)
	cd $thisdir
	for file in bin/*; do
		ln -sf "$thisdir/$file" ../bin/
	done
	;;

*)
	echo "Must supply entry point (all, download, binutils, no-red-zone, or gcc"
esac
