#ifndef ZYCHEATS_SGUYS_FUNCTIONS_H
#define ZYCHEATS_SGUYS_FUNCTIONS_H

#include <cstdint>

// Menu state
bool g_unlockLimitedCharacters = false;
bool g_unlockShopEventTabs = false;
bool g_zeroScoutAndCostumePrices = false;
bool g_removeAds = false;
bool g_skillGaugeAlwaysFull = false;

// ShopData / ShopUIItemData field offsets from dump.cs.
constexpr uintptr_t kOffShopDataShow = 0x18;
constexpr uintptr_t kOffShopDataShowAfterBuy = 0x1C;
constexpr uintptr_t kOffShopDataCostAmount = 0x78;
constexpr uintptr_t kOffShopDataLimitValue = 0x90;
constexpr uintptr_t kOffShopDataLimitAmount = 0x94;
constexpr uintptr_t kOffShopDataUnlockValue = 0xA0;
constexpr uintptr_t kOffShopUIItemDataShopData = 0x10;
constexpr uintptr_t kOffShopUIItemDataCanBuy = 0x30;
constexpr uintptr_t kOffShopUIItemItemData = 0x170;

// Function pointers for original methods
bool (*old_NetDataUtil_CanBuy)(void *shopData) = nullptr;
void (*old_ShopUIItem_UpdateData)(void *instance, void *scrollData) = nullptr;
bool (*old_RedNotiInfo_IsPossibleGoldShop)(void *instance) = nullptr;
bool (*old_RedNotiInfo_IsPossibleDailyShop)(void *instance) = nullptr;
bool (*old_NetDataUtil_IsPossiblePlayWorldTour)() = nullptr;
bool (*old_NetDataUtil_IsPossibleWorldTourShop)() = nullptr;

void Pointers() {}

void Patches() {
    // === ADS REMOVAL WITH REWARDS MODS ===
    PATCH_SWITCH("0x250DE58", "00008052C0035FD6", g_removeAds); // AdsManager_CheckInterstitialAdsStartCondition -> RETURN_FALSE
    PATCH_SWITCH("0x250D8BC", "00008052C0035FD6", g_removeAds); // AdsManager_StartInterstitialAd -> RETURN_FALSE
    PATCH_SWITCH("0x250D8C0", "20008052C0035FD6", g_removeAds); // AdsManager_VideoIsLoaded -> RETURN_TRUE

    // === SKILL GAUGE ALWAYS FULL MODS ===
    PATCH_SWITCH("0x24C4C48", "20008052C0035FD6", g_skillGaugeAlwaysFull); // BaseCharacterInfo_IsReadySkill -> RETURN_TRUE
    PATCH_SWITCH("0x24C9BA0", "C0035FD6", g_skillGaugeAlwaysFull); // BaseCharacterInfo_DecreaseSkillExp -> RETURN_VOID
    PATCH_SWITCH("0x2377490", "20008052C0035FD6", g_skillGaugeAlwaysFull); // MBaseCharacterInfo_IsReadySkill -> RETURN_TRUE
    PATCH_SWITCH("0x2378CEC", "C0035FD6", g_skillGaugeAlwaysFull); // MBaseCharacterInfo_DecreaseSkillExp -> RETURN_VOID
}

void PatchShopDataFields(void *shopData) {
    if (shopData == nullptr) return;

    if (g_unlockLimitedCharacters) {
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataShow) = 1;
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataShowAfterBuy) = 1;
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataLimitValue) = 0;
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataLimitAmount) = 999999;
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataUnlockValue) = 0;
    }

    if (g_zeroScoutAndCostumePrices) {
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataCostAmount) = 0;
    }
}

void PatchShopUIItemDataFields(void *itemData) {
    if (itemData == nullptr) return;

    if (g_zeroScoutAndCostumePrices) {
        *reinterpret_cast<uint8_t *>((uintptr_t) itemData + kOffShopUIItemDataCanBuy) = 1;
    }

    void *shopData = *reinterpret_cast<void **>((uintptr_t) itemData + kOffShopUIItemDataShopData);
    PatchShopDataFields(shopData);
}

// === SHOP & UNLOCKS MODS ===
bool NetDataUtil_CanBuy(void *shopData) {
    if (g_zeroScoutAndCostumePrices) return true;
    if (old_NetDataUtil_CanBuy != nullptr) return old_NetDataUtil_CanBuy(shopData);
    return false;
}

void ShopUIItem_UpdateData(void *instance, void *scrollData) {
    if (instance != nullptr) {
        void *itemData = *reinterpret_cast<void **>((uintptr_t) instance + kOffShopUIItemItemData);
        PatchShopUIItemDataFields(itemData);
    }
    if (old_ShopUIItem_UpdateData != nullptr) {
        old_ShopUIItem_UpdateData(instance, scrollData);
    }
}

bool RedNotiInfo_IsPossibleGoldShop(void *instance) {
    if (g_unlockShopEventTabs) return true;
    if (old_RedNotiInfo_IsPossibleGoldShop != nullptr) return old_RedNotiInfo_IsPossibleGoldShop(instance);
    return false;
}

bool RedNotiInfo_IsPossibleDailyShop(void *instance) {
    if (g_unlockShopEventTabs) return true;
    if (old_RedNotiInfo_IsPossibleDailyShop != nullptr) return old_RedNotiInfo_IsPossibleDailyShop(instance);
    return false;
}

bool NetDataUtil_IsPossiblePlayWorldTour() {
    if (g_unlockShopEventTabs) return true;
    if (old_NetDataUtil_IsPossiblePlayWorldTour != nullptr) return old_NetDataUtil_IsPossiblePlayWorldTour();
    return false;
}

bool NetDataUtil_IsPossibleWorldTourShop() {
    if (g_unlockShopEventTabs) return true;
    if (old_NetDataUtil_IsPossibleWorldTourShop != nullptr) return old_NetDataUtil_IsPossibleWorldTourShop();
    return false;
}

void Hooks() {
    // Zero Cost Daily/Costumes + Unlock Limited Characters
    HOOK("0x2445F10", NetDataUtil_CanBuy, old_NetDataUtil_CanBuy);
    HOOK("0x25031E8", ShopUIItem_UpdateData, old_ShopUIItem_UpdateData);

    // Unlock Shop/Event tabs
    HOOK("0x24F2AA8", RedNotiInfo_IsPossibleGoldShop, old_RedNotiInfo_IsPossibleGoldShop);
    HOOK("0x24F2828", RedNotiInfo_IsPossibleDailyShop, old_RedNotiInfo_IsPossibleDailyShop);
    HOOK("0x244CB4C", NetDataUtil_IsPossiblePlayWorldTour, old_NetDataUtil_IsPossiblePlayWorldTour);
    HOOK("0x244CC28", NetDataUtil_IsPossibleWorldTourShop, old_NetDataUtil_IsPossibleWorldTourShop);

    LOGI("%s", (const char *) OBFUSCATE("VolleyGirls stripped script hooks installed"));
}

#endif //ZYCHEATS_SGUYS_FUNCTIONS_H
