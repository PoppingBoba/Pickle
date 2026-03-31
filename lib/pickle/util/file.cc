#include <pickle/util/file.h>

#include <pickle/util/compiler.h>

namespace Pickle::util
{

void File::Initialize()
{
    _loadedImage = nullptr;
    _fsp = nullptr;
}

void File::Uninitialize()
{

}

EFI_STATUS
File::OpenRoot(
    EFI_HANDLE ImageHandle,
    EFI_FILE_PROTOCOL*& Root
)
{
    EFI_STATUS status = EFI_SUCCESS;

    status = BS->HandleProtocol(
        ImageHandle,
        &LoadedImageProtocol,
        reinterpret_cast<VOID**>(&_loadedImage)
    );
    
    if (EFI_ERROR(status))
    {
        Print(W("Failed to get LoadedImageProtocol.... [%r]\r\n"), status);
        goto done;
    }

    status = BS->HandleProtocol(
        _loadedImage->DeviceHandle,
        &FileSystemProtocol,
        reinterpret_cast<VOID**>(&_fsp)
    );

    if (EFI_ERROR(status))
    {
        Print(W("Failed to get Filesystem Protocol... [%r]\r\n"), status);
        goto done;
    }

    status = _fsp->OpenVolume(
        _fsp,
        &Root
    );

done:
    return status;
}

EFI_STATUS
File::ReadFile(
    EFI_HANDLE ImageHandle,
    const CHAR16* Path,
    VOID** OutBuffer,
    UINTN* OutSize
)
{
    EFI_STATUS status = EFI_SUCCESS;

    EFI_FILE_PROTOCOL* Root = nullptr;
    EFI_FILE_PROTOCOL* File = nullptr;
    EFI_FILE_INFO* FileInfo = nullptr;
    UINTN FileInfoSize = 0;
    // VOID* Buffer = nullptr;

    if (!OutBuffer || !OutSize)
        return EFI_INVALID_PARAMETER;

    status = OpenRoot(
        ImageHandle,
        Root
    );
    if (EFI_ERROR(status))
    {
        Print(W("Failed to Open Root... [%r]\n"), status);
        goto done;
    }

    status = Root->Open(
        Root,
        &File,
        const_cast<CHAR16*>(Path),
        EFI_FILE_MODE_READ,
        0
    );
    if (EFI_ERROR(status))
    {
        Print(W("Failed to Root->Open... [%r]\n"), status);
        goto done;
    }

    status = File->GetInfo(
        File,
        &GenericFileInfo,
        &FileInfoSize,
        NULL
    );
    if (EFI_ERROR(status) &&
        status != EFI_BUFFER_TOO_SMALL)
    {
        Print(W("Failed to Get File Info... [%r]\n"), status);
        goto done;
    }

    status = BS->AllocatePool(
        EfiLoaderData,
        FileInfoSize,
        reinterpret_cast<VOID**>(&FileInfo)
    );
    if (EFI_ERROR(status))
    {
        Print(W("Failed to Allocate Pool... [%r]\n"), status);
        goto done;
    }

    status = File->GetInfo(
        File,
        &GenericFileInfo,
        &FileInfoSize,
        FileInfo
    );

    *OutSize = FileInfo->FileSize;

done:
    if (FileInfo)
        BS->FreePool(FileInfo);

    if (File)
        File->Close(File);

    if (Root)
        Root->Close(Root);

    return status;
}

};