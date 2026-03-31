#ifndef __PICKLE_UTIL_FILE_H__
#define __PICKLE_UTIL_FILE_H__

#include <efi.h>
#include <efilib.h>

#include <pickle/ext/Object.h>

namespace Pickle::util
{

class File : public ::Pickle::ext::Object
{
    EFI_LOADED_IMAGE* _loadedImage;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* _fsp;

public:
    void Initialize() override;
    void Uninitialize() override;

private:
    EFI_STATUS OpenRoot(EFI_HANDLE ImageHandle, EFI_FILE_PROTOCOL*& Root);

public:
    EFI_STATUS ReadFile(EFI_HANDLE ImageHandle, const CHAR16* Path, VOID** OutBuffer, UINTN* OutSize);
};

};

#endif