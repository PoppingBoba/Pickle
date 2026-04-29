#include <pickle/amoeba/elf.h>

#include <pickle/util/compiler.h>

namespace Pickle::amoeba
{

EFI_STATUS Elf::ValidateElf()
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
    }

done:
    return ret;
}

EFI_STATUS Elf::LoadImage(Pickle::util::DynamicArray<UINT8>& Image)
{
    EFI_STATUS ret = EFI_SUCCESS;

    _ehdr = reinterpret_cast<Elf64_Ehdr*>(Image.Data());

    ret = this->ValidateElf();

    return ret;
}

};