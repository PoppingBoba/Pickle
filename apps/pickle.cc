
#include <efi.h>
#include <efilib.h>

extern "C"
{

// For C
EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);

};

#define W(T) reinterpret_cast<const CHAR16*>(L##T)

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    Print(W("Hello from Pickle!\r\n"));

    return EFI_SUCCESS;
}
