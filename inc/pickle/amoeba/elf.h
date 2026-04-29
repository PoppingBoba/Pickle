#ifndef __PICKLE_ELF_H__
#define __PICKLE_ELF_H__

#include <efi.h>

#include <pickle/amoeba/small_elf.h>
#include <pickle/ext/Object.h>
#include <pickle/util/dynamic_array.h>

namespace Pickle::amoeba
{

class Elf : public Pickle::ext::Object
{
    Elf64_Ehdr* _ehdr;

private:
    EFI_STATUS ValidateElf();
    
public:
    EFI_STATUS LoadImage(Pickle::util::DynamicArray<UINT8>& Image);

public:
    void Initialize() override
    {
        _ehdr = nullptr;
    }

    void Uninitialize() override
    {
        _ehdr = nullptr;
    }

    PickleDeclareCtorAndDtor(Elf);
};

};

#endif