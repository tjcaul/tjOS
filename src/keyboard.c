#include "keyboard.h"

void wait_for_key (wchar_t c) {
	EFI_INPUT_KEY key;
	EFI_STATUS status = ST->ConIn->Reset(ST->ConIn, FALSE);
	while (key.UnicodeChar != c) {
		while ((status = ST->ConIn->ReadKeyStroke(ST->ConIn, &key)) == EFI_NOT_READY) ;
	}
}
