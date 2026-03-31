#ifndef __PICKLE_ELF_H__
#define __PICKLE_ELF_H__

#include <efi.h>
#include <pickle/small_elf.h>

namespace Pickle
{

class ElfReader
{
    Elf64_Ehdr* hdr;

    EFI_PHYSICAL_ADDRESS base;
    UINTN size;
    EFI_PHYSICAL_ADDRESS entry;

private:
    EFI_STATUS ValidateElf();
    
public:
    EFI_STATUS LoadImage();
};

};

#endif