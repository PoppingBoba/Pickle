
#include <efi.h>
#include <efilib.h>

#include <pickle/pickle.h>
#include <pickle/util/file.h>

extern "C"
{

// For C
EFI_STATUS
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);

};

void pickleTest(EFI_HANDLE ImageHandle)
{
    EFI_STATUS status = EFI_SUCCESS;

    VOID* Data;
    UINTN DataSize;

    Pickle::util::File picklefile;
    
    picklefile.Initialize();

    status = picklefile.ReadFile(
        ImageHandle,
        W("\\test.txt"),
        &Data,
        &DataSize
    );
    if (EFI_ERROR(status))
    {
        Print(W("Failed to Read File... [%r]\n"), status);
    }
    else
    {
        Print(W("File Size: %lu\n"), DataSize);
    }

    picklefile.Uninitialize();
}

EFI_STATUS
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{

    InitializeLib(ImageHandle, SystemTable);

    Print(W("Hello from Pickle!\r\n"));

    pickleTest(ImageHandle);

    return EFI_SUCCESS;
}
