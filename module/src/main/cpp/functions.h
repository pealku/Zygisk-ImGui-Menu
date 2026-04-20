#ifndef ZYCHEATS_SGUYS_FUNCTIONS_H
#define ZYCHEATS_SGUYS_FUNCTIONS_H

#include <cstdint>

// Menu state
bool g_unlockLimitedCharacters = false;
bool g_unlockShopEventTabs = false;
bool g_removeAds = false;

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
void (*old_ShopUIItem_UpdateData)(void *instance, void *scrollData) = nullptr;  

void Pointers() {}

void Patches() {
    // === UNLOCK LIMITED CHARACTERS MOD ===
    PATCH_SWITCH("0x2445F10", "20008052C0035FD6", g_unlockLimitedCharacters); // NetDataUtil_CanBuy -> RETURN_TRUE

    // === UNLOCK SHOP/EVENT TABS MOD ===
    PATCH_SWITCH("0x24F2AA8", "20008052C0035FD6", g_unlockShopEventTabs); // RedNotiInfo_IsPossibleGoldShop -> RETURN_TRUE
    PATCH_SWITCH("0x24F2828", "20008052C0035FD6", g_unlockShopEventTabs); // RedNotiInfo_IsPossibleDailyShop -> RETURN_TRUE
    PATCH_SWITCH("0x244CB4C", "20008052C0035FD6", g_unlockShopEventTabs); // NetDataUtil_IsPossiblePlayWorldTour -> RETURN_TRUE
    PATCH_SWITCH("0x244CC28", "20008052C0035FD6", g_unlockShopEventTabs); // NetDataUtil_IsPossibleWorldTourShop -> RETURN_TRUE

    // === ADS REMOVAL WITH REWARDS MODS ===
    PATCH_SWITCH("0x250DE58", "00008052C0035FD6", g_removeAds); // AdsManager_CheckInterstitialAdsStartCondition -> RETURN_FALSE
    PATCH_SWITCH("0x250D8BC", "00008052C0035FD6", g_removeAds); // AdsManager_StartInterstitialAd -> RETURN_FALSE
    PATCH_SWITCH("0x250D8C0", "20008052C0035FD6", g_removeAds); // AdsManager_VideoIsLoaded -> RETURN_TRUE
}

void PatchShopDataFields(void *shopData) {
    if (shopData == nullptr) return;

    if (g_unlockLimitedCharacters) {
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataShow) = 1;
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataShowAfterBuy) = 1;
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataCostAmount) = 0;
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataLimitValue) = 0;
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataLimitAmount) = 999999;
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataUnlockValue) = 0;
    }
}

void PatchShopUIItemDataFields(void *itemData) {
    if (itemData == nullptr) return;

    if (g_unlockLimitedCharacters) {
        *reinterpret_cast<uint8_t *>((uintptr_t) itemData + kOffShopUIItemDataCanBuy) = 1;
    }

    void *shopData = *reinterpret_cast<void **>((uintptr_t) itemData + kOffShopUIItemDataShopData);
    PatchShopDataFields(shopData);
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

void Hooks() {
    HOOK("0x25031E8", ShopUIItem_UpdateData, old_ShopUIItem_UpdateData);

    LOGI("%s", (const char *) OBFUSCATE("VolleyGirls stripped script hooks installed"));
}

#endif //ZYCHEATS_SGUYS_FUNCTIONS_H
