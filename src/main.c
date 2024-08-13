#include <stddef.h>
#include "efi.h"
#include "efilib.h"

#include "display.h"
#include "keyboard.h"

#include "font/mini_5x7.h"
#include "font/smooth_15x27.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_STATUS status;
 
	ST = SystemTable;
	BS = ST->BootServices;
 
	init_gop(-1);

	draw_rect(0, 0, 1000, 200, 0xffffff);
	//test_gradient();
	test_ascii(0, 20, &smooth_15x27, 1, 0x000000, COLOUR_TRANSPARENT);
	wait_for_key(L'e');
	return status;
}
