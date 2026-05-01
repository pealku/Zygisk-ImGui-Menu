#ifndef ZYCHEATS_SARA_FUNCTIONS_H
#define ZYCHEATS_SARA_FUNCTIONS_H

#include "sara_whitespike.h"

// toggle bindings for the menu
bool saraWhiteSpike = false;

void sara_init() {
    // called once after libyoyo.so base is found
    if (g_saraCtx.found) return;

    KittyMemory::ProcMap yoyoMap = KittyMemory::getLibraryBaseMap("libyoyo.so");
    if (!yoyoMap.isValid()) return;

    uintptr_t base = yoyoMap.startAddress;
    size_t    size = yoyoMap.length;

    if (sara_find_and_patch(base, size)) {
        LOGI("SpikerSara found at %p", (void*)g_saraCtx.funcAddr);
    } else {
        LOGE("SpikerSara NOT found in libyoyo.so");
    }
}

void sara_patch_toggle() {
    if (!g_saraCtx.found) return;

    if (saraWhiteSpike && !g_saraCtx.enabled) {
        sara_apply();
        LOGI("WhiteSpike applied to Sara");
    } else if (!saraWhiteSpike && g_saraCtx.enabled) {
        sara_restore();
        LOGI("WhiteSpike removed from Sara");
    }
}

#endif // ZYCHEATS_SARA_FUNCTIONS_H