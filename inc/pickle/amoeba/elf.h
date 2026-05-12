#ifndef __PICKLE_ELF_H__
#define __PICKLE_ELF_H__

#include <efi.h>

#include <pickle/amoeba/small_elf.h>
#include <pickle/util/dynamic_array.h>

#define SEGMENT_INFO_DEFAULT_START_ADDR (~0ULL)
#define SEGMENT_INFO_DEFAULT_END_ADDR   (0)

namespace Pickle::amoeba
{

struct ElfSegmentInfo
{
    EFI_PHYSICAL_ADDRESS startAddr;
    EFI_PHYSICAL_ADDRESS endAddr;
};

class Elf
{
    Elf64_Ehdr* _ehdr;
    Elf64_Phdr* _phdr;

    EFI_PHYSICAL_ADDRESS _loadBaseAddr;
    UINTN _loadBasePages;

private:
    EFI_STATUS ValidateElf(UINTN elfSize);
    EFI_STATUS FindLOADSegment(ElfSegmentInfo& segInfo);

public:
    EFI_STATUS LoadImage(
        Pickle::util::DynamicArray<UINT8>& Image,
        EFI_PHYSICAL_ADDRESS& outEntry
    );

public:
    Elf();
    ~Elf();
};

};

#endif