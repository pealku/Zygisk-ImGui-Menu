//
// Created by lbert on 2/9/2023.
//
#ifndef ZYGISKPG_MISC_H
#define ZYGISKPG_MISC_H

#include "Includes/Dobby/dobby.h"
#include "KittyMemory/KittyMemory.h"
#include "KittyMemory/KittyScanner.h"
#include "KittyMemory/MemoryPatch.h"
#include "Include/obfuscate.h"

using KittyMemory::ProcMap;

static ProcMap g_yoyoBaseMap;

#define HOOK(offset, ptr, orig) hook((void *)(g_yoyoBaseMap.startAddress + string2Offset(OBFUSCATE(offset))), (void *)ptr, (void **)&orig)

uintptr_t string2Offset(const char *c) {
    return strtoull(c, nullptr, 16);
}

#endif //ZYGISKPG_MISC_H