#include <pickle/amoeba/elf.h>

#include <efidef.h>

#include <pickle/util/allocator.h>
#include <pickle/util/compiler.h>

namespace Pickle::amoeba
{

Elf::Elf()
{
    _ehdr = nullptr;
    _phdr = nullptr;

    _loadBaseAddr = 0;
    _loadBasePages = 0;
}

Elf::~Elf()
{
    _loadBasePages = 0;
    _loadBaseAddr = 0;

    _phdr = nullptr;
    _ehdr = nullptr;
}

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

    _loadBaseAddr = segInfo.startAddr;

    return status;
}

EFI_STATUS Elf::LoadImage(
    Pickle::util::DynamicArray<UINT8>& Image,
    EFI_PHYSICAL_ADDRESS& outEntry
)
{
    EFI_STATUS status = EFI_SUCCESS;

    UINTN pages = 0;
    ElfSegmentInfo segInfo = { SEGMENT_INFO_DEFAULT_START_ADDR, SEGMENT_INFO_DEFAULT_END_ADDR };

    _ehdr = reinterpret_cast<Elf64_Ehdr*>(Image.Data());

    status = this->ValidateElf(Image.Size());
    if (EFI_ERROR(status))
    {
        Print(W("Failed to validate ELF File...\r\n"));
        goto load_image_fail;
    }

    _phdr = reinterpret_cast<Elf64_Phdr*>(Image.Data() + _ehdr->e_phoff);

    status = this->FindLOADSegment(segInfo);
    if (EFI_ERROR(status))
    {
        Print(W("Failed to find Load Segment...\r\n"));
        goto load_image_fail;
    }

    pages = SIZE_TO_PAGES(segInfo.endAddr - segInfo.startAddr);

    status = BS->AllocatePages(
        AllocateAddress,
        EfiLoaderCode,
        pages,
        &_loadBaseAddr
    );

    if (EFI_ERROR(status))
    {
        Print(W("Failed to Allocate Page....[Addr: 0x%lx Error: %r]\r\n"), _loadBaseAddr, status);
        goto load_image_fail;
    }

    BS->SetMem(
        reinterpret_cast<VOID*>(segInfo.startAddr),
        (segInfo.endAddr - segInfo.startAddr),
        0
    );

    for (UINTN i = 0; i < _ehdr->e_phnum; i++)
    {
        Elf64_Phdr *p = reinterpret_cast<Elf64_Phdr*>(reinterpret_cast<UINT8*>(_phdr + i * _ehdr->e_phentsize));

        if (p->p_type != PT_LOAD)
            continue;

        if (p->p_offset + p->p_filesz > Image.Size())
        {
            Print(W("Invalid PT_LOAD file range...\r\n"));
            status = EFI_LOAD_ERROR;
            goto load_image_fail;
        }

        CopyMem(
            reinterpret_cast<VOID*>(p->p_paddr),
            (Image.Data() + p->p_offset),
            p->p_filesz
        );
    }

    outEntry = _ehdr->e_entry;

    for (UINTN i = 0; i < _ehdr->e_phnum; i++)
    {
        auto *p = reinterpret_cast<Elf64_Phdr*>(reinterpret_cast<UINT8*>(_phdr) + i * _ehdr->e_phentsize);

        if (p->p_type != PT_LOAD)
            continue;

        if ((_ehdr->e_entry >= p->p_vaddr) &&
            (_ehdr->e_entry < p->p_vaddr + p->p_memsz))
        {
            outEntry = p->p_paddr + (_ehdr->e_entry - p->p_vaddr);
            break;
        }
    }

    Print(W("LK.elf Loaded: 0x%lx - 0x%lx, entry=0x%lx\r\n"), segInfo.startAddr, segInfo.endAddr, outEntry);

    return status;

load_image_fail:
    if (_loadBaseAddr)
    {
        BS->FreePages(_loadBaseAddr, pages);
    }

    return status;
}

};