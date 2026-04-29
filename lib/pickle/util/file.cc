#include <pickle/util/file.h>

#include <pickle/util/allocator.h>
#include <pickle/util/compiler.h>
#include <pickle/util/dynamic_array.h>
#include <pickle/util/string.h>

namespace Pickle::util
{

void File::Initialize()
{
    _root = nullptr;
    _file = nullptr;
    _loadedImage = nullptr;
    _fsp = nullptr;

    _fileBuf = nullptr;
}

void File::Uninitialize()
{
    this->Close();
}

EFI_STATUS
File::OpenRoot(
    EFI_HANDLE ImageHandle
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
        &_root
    );

done:
    return status;
}

EFI_STATUS
File::GetFileInfo(VOID)
{
    EFI_STATUS status = EFI_SUCCESS;

    UINTN infoSize = 0;

    status = _file->GetInfo(
        _file,
        &GenericFileInfo,
        &infoSize,
        nullptr
    );
    if (EFI_ERROR(status) && 
        (status != EFI_BUFFER_TOO_SMALL))
    {
        Print(W("Failed to get GenericFileInfo size... [%r]\r\n"), status);
        goto done;
    }

    status = _fileInfo.Allocate(infoSize);
    if (EFI_ERROR(status))
    {
        Print(W("Failed to allocate FileInfo... [%r]\r\n"), status);
        goto done;
    }

    status = _file->GetInfo(
        _file,
        &GenericFileInfo,
        &infoSize,
        _fileInfo.Data()
    );

done:
    return status;
}

EFI_STATUS
File::Open(
    EFI_HANDLE ImageHandle,
    const CHAR16* Path,
    UINT64 OpenMode
)
{
    EFI_STATUS status = EFI_SUCCESS;

    // Open the root volume if the program doesn't open before
    if (!_root) 
    {
        status = OpenRoot(ImageHandle);
        if (EFI_ERROR(status))
        {
            Print(W("Failed to Open Root... [%r]\n"), status);
            goto done;
        }
    }

    // Close the file if file already open
    if (_file)
    {
        _file->Close(_file);
        _file = nullptr;
    }

    status = _root->Open(
        _root,
        &_file,
        const_cast<CHAR16*>(Path),
        OpenMode,
        0
    );
    if (EFI_ERROR(status))
    {
        Print(W("Failed to Open File... [%r]\r\n"), status);
        goto done;
    }

    status = this->GetFileInfo();

done:
    return status;
}

VOID
File::Close(VOID)
{
    if (_fileBuf)
    {
        BS->FreePool(_fileBuf);
    }

    if (_file)
    {
        _file->Close(_file);
        _file = nullptr;
    }

    if (_root)
    {
        _root->Close(_root);
        _root = nullptr;
    }
}

EFI_STATUS
File::Read(
    Pickle::util::DynamicArray<UINT8>& OutBuffer,
    UINTN& ReadSize
)
{
    EFI_STATUS status = EFI_SUCCESS;

    status = OutBuffer.Allocate(ReadSize);
    if (EFI_ERROR(status))
    {
        Print(W("Failed to allocate Output Buffer... [%d]\r\n"), status);
        return status;
    }

    status = _file->Read(
        _file,
        &ReadSize,
        OutBuffer.Data()
    );

    return status;
}

EFI_STATUS
File::Read(Pickle::util::DynamicArray<UINT8>& OutBuffer)
{
    EFI_STATUS status = EFI_SUCCESS;

    UINTN size = this->Size();
    status = this->Read(OutBuffer, size);

    return status;
}

UINT64
File::Size(VOID)
{
    return (_fileInfo != nullptr) ? 
        reinterpret_cast<EFI_FILE_INFO*>(_fileInfo.Data())->FileSize : 
        0;
}

};