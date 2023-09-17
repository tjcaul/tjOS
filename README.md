# tjOS

Proof of concept for `MacBookPro15,4`
A bootable image showing **keyboard input** and **graphics**.
Includes a QEMU setup to run it virtually as well.

## Building
``
git clone --depth 1 https://github.com/tjcaul/tjOS
cd tjOS/
tools/compiler/build.sh
tools/compiler/clean.sh
tools/mkfont/build.sh
scripts/compile.sh
```

### Installing
```
scripts/install.sh /dev/diskx
``

### Running in QEMU
```
scripts/mkhd.sh
scripts/qemu.sh
```

## Scripts
`compile.sh`: compile files in `src/` to an executable (`build/BOOTX64.EFI`)
`install.sh`: copy `BOOTX64.EFI` to a drive
`mkhd.sh`: build a virtual drive for QEMU and install tjOS on it
`qemu.sh`: boot the virtual drive in QEMU
Run `scripts/qemu.sh shell` to boot to UEFI shell instead for troubleshooting.
