
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

    // VOID* Data;
    // UINTN DataSize;

    Pickle::util::File picklefile;

    status = picklefile.Open(
        ImageHandle,
        W("\\test.txt")
    );
    if (EFI_ERROR(status))
    {
        Print(W("Failed to Open File... [%r]\n"), status);
    }
    else
    {
        Print(W("Open Success\r\n"));
    }

    UINTN size = 30;
    Pickle::util::DynamicArray<UINT8> data;

    status = picklefile.Read(data, size);
    if (EFI_ERROR(status))
    {
        Print(W("Failed to Read File... [%r]\n"), status);
    }

    for (UINTN i = 0; i < size; i++)
    {
        Print(W("Read Test [%d] : %c \r\n"), i, data.Data()[i]);
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
