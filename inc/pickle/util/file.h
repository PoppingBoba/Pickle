/*
 *
 *              file.h
 * 
 * Desc: C++ Style file api for Pickle framework
 * Author: Nakada Tokumei <nakada_tokumei@protonmail.com>
 * 
 */

#ifndef __PICKLE_UTIL_FILE_H__
#define __PICKLE_UTIL_FILE_H__

#include <efi.h>
#include <efilib.h>

#include <pickle/ext/Object.h>
#include <pickle/util/dynamic_array.h>

namespace Pickle::util
{

class File : public ::Pickle::ext::Object
{
    // Root Voulume
    EFI_FILE_PROTOCOL* _root;
    // File
    EFI_FILE_PROTOCOL* _file;
    EFI_LOADED_IMAGE* _loadedImage;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* _fsp;

    CHAR8* _fileBuf;

public:
    void Initialize() override;
    void Uninitialize() override;

private:
    EFI_STATUS OpenRoot(EFI_HANDLE ImageHandle);

public:
    EFI_STATUS Open(EFI_HANDLE ImageHandle, const CHAR16* Path, UINT64 OpenMode = (EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE));
    VOID Close(VOID);

    EFI_STATUS Read(Pickle::util::DynamicArray<UINT8>& OutBuffer, UINTN& ReadSize);

    PickleDeclareCtorAndDtor(File)
};

};

#endif