
#include <efi.h>
#include <efilib.h>

#include <pickle/pickle.h>
#include <pickle/amoeba/elf.h>
#include <pickle/util/file.h>

extern "C"
{

// For No mangling function declaration
EFI_STATUS
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);

};

void pickleTest(EFI_HANDLE ImageHandle)
{
    EFI_STATUS status = EFI_SUCCESS;

    Pickle::util::File picklefile;

    status = picklefile.Open(
        ImageHandle,
        W("\\lk.elf")
    );
    if (EFI_ERROR(status))
    {
        Print(W("Failed to Open File... [%r]\n"), status);
    }
    else
    {
        Print(W("Open Success\r\n"));
    }

    Pickle::util::DynamicArray<UINT8> data;

    status = picklefile.Read(data);
    if (EFI_ERROR(status))
    {
        Print(W("Failed to Read File... [%r]\n"), status);
    }

    Print(W("File Size: %lu\r\n"), picklefile.Size());

    Pickle::amoeba::Elf elf;
    status = elf.LoadImage(data);
    if (EFI_ERROR(status))
    {
        Print(W("Failed to Load ELF Image...[%r]\r\n"), status);
    }

}

EFI_STATUS
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{

    InitializeLib(ImageHandle, SystemTable);

    Print(W("Hello from Pickle!\r\n"));

    pickleTest(ImageHandle);

    return EFI_SUCCESS;
}
