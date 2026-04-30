#include <pickle/amoeba/elf.h>

#include <pickle/util/compiler.h>

namespace Pickle::amoeba
{

EFI_STATUS Elf::ValidateElf(UINTN elfSize)
{
    EFI_STATUS ret = EFI_SUCCESS;

    if (!_ehdr)
    {
        ret = EFI_INVALID_PARAMETER;
        goto done;
    }

    if (_ehdr->e_magic != ELFMAG)
    {
        ret = EFI_UNSUPPORTED;
        Print(W("ELF Magic is wrong now :O [0x%08x 0x%08x]\r\n"), _ehdr->e_magic, ELFMAG);
        goto done;
    }

    if ((_ehdr->e_ident[0] != ELFCLASS64) || 
        (_ehdr->e_ident[1] != ELFDATA2LSB))
    {
        ret = EFI_UNSUPPORTED;
        Print(W("Unsupported ELF class/data encoding...\r\n"));
        goto done;
    }

    if ((_ehdr->e_type != ET_EXEC) || 
        (_ehdr->e_machine != EM_X86_64))
    {
        ret = EFI_UNSUPPORTED;
        Print(W("Unsupported ELF type or machine...\r\n"));
        goto done;
    }

    if (_ehdr->e_phoff + (static_cast<UINT64>(_ehdr->e_phnum) * _ehdr->e_phentsize) > elfSize)
    {
        ret = EFI_LOAD_ERROR;
        Print(W("Invalid Program Header Table...\r\n"));
        goto done;
    }

done:
    return ret;
}

EFI_STATUS Elf::FindLOADSegment(ElfSegmentInfo& segInfo)
{
    EFI_STATUS status = EFI_SUCCESS;
    Elf64_Phdr* p = nullptr;

    EFI_PHYSICAL_ADDRESS segStart = 0;
    EFI_PHYSICAL_ADDRESS segEnd = 0;
    
    UINTN i = 0;

    for (i = 0; i < _ehdr->e_phnum; i++)
    {
        p = reinterpret_cast<Elf64_Phdr*>(reinterpret_cast<UINT8*>(_phdr) + (i * _ehdr->e_phentsize));

        if (p->p_type != PT_LOAD)
            continue;
        
        segStart = ALIGN_DOWN(p->p_paddr, PAGE_SIZE);
        segEnd = ALIGN_UP(p->p_paddr + p->p_memsz, PAGE_SIZE);

        if (segStart < segInfo.startAddr)
            segInfo.startAddr = segStart;

        if (segEnd > segInfo.endAddr)
            segInfo.endAddr = segEnd;
    }

    if ((segInfo.startAddr == SEGMENT_INFO_DEFAULT_START_ADDR) &&
        (segInfo.endAddr == SEGMENT_INFO_DEFAULT_END_ADDR))
    {
        Print(W("Failed to Find PT_LOAD segment...\r\n"));
        status = EFI_LOAD_ERROR;
    }

    return status;
}

EFI_STATUS Elf::LoadImage(Pickle::util::DynamicArray<UINT8>& Image)
{
    EFI_STATUS ret = EFI_SUCCESS;
    ElfSegmentInfo segInfo = { SEGMENT_INFO_DEFAULT_START_ADDR, SEGMENT_INFO_DEFAULT_END_ADDR };

    _ehdr = reinterpret_cast<Elf64_Ehdr*>(Image.Data());

    ret = this->ValidateElf(Image.Size());
    if (EFI_ERROR(ret))
    {
        Print(W("Failed to validate ELF File...\r\n"));
        goto done;
    }

    _phdr = reinterpret_cast<Elf64_Phdr*>(Image.Data() + _ehdr->e_phoff);

    ret = this->FindLOADSegment(segInfo);
    if (EFI_ERROR(ret))
    {
        Print(W("Failed to find Load Segment...\r\n"));
        goto done;
    }

done:
    return ret;
}

};