
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

typedef VOID (*lk_entry_t)();

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

    EFI_PHYSICAL_ADDRESS entryAddr;
    Pickle::amoeba::Elf elf;
    status = elf.LoadImage(data, entryAddr);
    if (EFI_ERROR(status))
    {
        Print(W("Failed to Load ELF Image...[%r]\r\n"), status);
    }

    auto lkEntry = reinterpret_cast<lk_entry_t>(entryAddr);
    lkEntry();
    for (;;) 
    {
        __asm__ __volatile__("hlt");
    }

}

EFI_STATUS
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{

    InitializeLib(ImageHandle, SystemTable);

    Print(W("Hello from Pickle!\r\n"));

    pickleTest(ImageHandle);

    Print(W("Bye from Pickle!\r\n"));

    return EFI_SUCCESS;
}
