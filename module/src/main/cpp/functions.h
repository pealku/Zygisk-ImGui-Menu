#ifndef ZYCHEATS_SGUYS_FUNCTIONS_H
#define ZYCHEATS_SGUYS_FUNCTIONS_H

#include <cstdint>

// Menu state
bool g_unlockLimitedCharacters = false;
bool g_unlockShopEventTabs = false;
bool g_zeroScoutAndCostumePrices = false;
bool g_beachMatchVictoryReward = false;
bool g_noStaminaDecrease = false;
bool g_removeAds = false;
int g_staminaLockValue = 99;

// ShopUIItem internals from dump.cs.
constexpr uintptr_t kOffShopUIItemItemData = 0x170;

// ShopData / ShopUIItemData field offsets from dump.cs.
constexpr uintptr_t kOffShopDataShow = 0x18;
constexpr uintptr_t kOffShopDataShowAfterBuy = 0x1C;
constexpr uintptr_t kOffShopDataCostAmount = 0x78;
constexpr uintptr_t kOffShopDataLimitValue = 0x90;
constexpr uintptr_t kOffShopDataLimitAmount = 0x94;
constexpr uintptr_t kOffShopDataUnlockValue = 0xA0;
constexpr uintptr_t kOffShopUIItemDataShopData = 0x10;
constexpr uintptr_t kOffShopUIItemDataCanBuy = 0x30;

// UserData stamina/entry fields from dump.cs.
constexpr uintptr_t kOffUserDataStamina = 0x16C; // st
constexpr uintptr_t kOffUserDataStaminaTime = 0x170; // stt
constexpr uintptr_t kOffUserDataStaminaAd = 0x174; // stad
constexpr uintptr_t kOffUserDataStaminaAdTime = 0x178; // stadt
constexpr uintptr_t kOffUserDataStaminaBeach = 0x17C; // stb
constexpr uintptr_t kOffUserDataStaminaBeachTime = 0x180; // stbt
constexpr uintptr_t kOffUserDataGoldMatchCount = 0x18C; // goldM
constexpr uintptr_t kOffUserDataWorldTourMatchCount = 0x1F4; // wtM
constexpr uintptr_t kOffUserDataCostumeMatchCount = 0x250; // cmM

// Direct call targets (RVA from dump.cs).
constexpr uintptr_t kOffNetDataUtilGetUserData = 0x2432E1C;

void *(*NetDataUtil_get_UserData)() = nullptr;

// Shop / tabs hooks.
bool (*old_NetDataUtil_CanBuy)(void *shopData) = nullptr;
void (*old_ShopUIItem_UpdateData)(void *instance, void *scrollData) = nullptr;
void (*old_ShopUIItem_BuyDailyItem)(void *instance) = nullptr;
void (*old_ShopUIItem_BuyShopItem)(void *instance) = nullptr;
bool (*old_RedNotiInfo_IsPossibleGoldShop)(void *instance) = nullptr;
bool (*old_RedNotiInfo_IsPossibleDailyShop)(void *instance) = nullptr;
bool (*old_NetDataUtil_IsPossiblePlayWorldTour)() = nullptr;
bool (*old_NetDataUtil_IsPossibleWorldTourShop)() = nullptr;

// Match/stamina hooks.
void *(*old_GameManager_CoFinishCostumeMatch)(void *instance, bool isWin) = nullptr;
void (*old_LeaguePopup_OnLeagueStartClick)(void *instance) = nullptr;
void (*old_CostumeMatchPopup_OnStartClick)(void *instance) = nullptr;

// Ad hooks.
bool (*old_AdsManager_CheckInterstitialAdsStartCondition)(void *instance) = nullptr;
void (*old_AdsManager_ShowInterstitial)(void *instance) = nullptr;
void (*old_AdsManager_StartInterstitialAd)(void *instance) = nullptr;

void PatchShopDataFields(void *shopData) {
    if (shopData == nullptr) {
        return;
    }

    if (g_unlockLimitedCharacters) {
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataShow) = 1;
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataShowAfterBuy) = 1;
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataLimitValue) = 0;
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataLimitAmount) = 999999;
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataUnlockValue) = 0;
    }

    if (g_zeroScoutAndCostumePrices || g_unlockLimitedCharacters) {
        *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataCostAmount) = 0;
    }
}

void PatchShopUIItemDataFields(void *itemData) {
    if (itemData == nullptr) {
        return;
    }

    if (g_unlockLimitedCharacters || g_zeroScoutAndCostumePrices) {
        *reinterpret_cast<uint8_t *>((uintptr_t) itemData + kOffShopUIItemDataCanBuy) = 1;
    }

    void *shopData = *reinterpret_cast<void **>((uintptr_t) itemData + kOffShopUIItemDataShopData);
    PatchShopDataFields(shopData);
}

void ApplyStaminaPatch() {
    if (!g_noStaminaDecrease || NetDataUtil_get_UserData == nullptr) {
        return;
    }

    void *userData = NetDataUtil_get_UserData();
    if (userData == nullptr) {
        return;
    }

    if (g_staminaLockValue < 1) {
        g_staminaLockValue = 1;
    }
    if (g_staminaLockValue > 9999) {
        g_staminaLockValue = 9999;
    }

    *reinterpret_cast<int32_t *>((uintptr_t) userData + kOffUserDataStamina) = g_staminaLockValue;
    *reinterpret_cast<int32_t *>((uintptr_t) userData + kOffUserDataStaminaAd) = g_staminaLockValue;
    *reinterpret_cast<int32_t *>((uintptr_t) userData + kOffUserDataStaminaBeach) = g_staminaLockValue;

    // Keep recharge timers/counters reset so they do not block match start flows.
    *reinterpret_cast<int32_t *>((uintptr_t) userData + kOffUserDataStaminaTime) = 0;
    *reinterpret_cast<int32_t *>((uintptr_t) userData + kOffUserDataStaminaAdTime) = 0;
    *reinterpret_cast<int32_t *>((uintptr_t) userData + kOffUserDataStaminaBeachTime) = 0;

    // Some modes gate entry by played-count fields rather than direct stamina.
    *reinterpret_cast<int32_t *>((uintptr_t) userData + kOffUserDataGoldMatchCount) = 0;
    *reinterpret_cast<int32_t *>((uintptr_t) userData + kOffUserDataWorldTourMatchCount) = 0;
    *reinterpret_cast<int32_t *>((uintptr_t) userData + kOffUserDataCostumeMatchCount) = 0;
}

void Pointers() {
    NetDataUtil_get_UserData = reinterpret_cast<void *(*)()>(
            g_il2cppBaseMap.startAddress + kOffNetDataUtilGetUserData);
}

void Patches() {
    ApplyStaminaPatch();
}

bool NetDataUtil_CanBuy(void *shopData) {
    PatchShopDataFields(shopData);

    if (g_unlockLimitedCharacters) {
        return true;
    }

    if (old_NetDataUtil_CanBuy != nullptr) {
        return old_NetDataUtil_CanBuy(shopData);
    }
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

    if (instance != nullptr) {
        void *itemData = *reinterpret_cast<void **>((uintptr_t) instance + kOffShopUIItemItemData);
        PatchShopUIItemDataFields(itemData);
    }
}

void ShopUIItem_BuyDailyItem(void *instance) {
    if (instance != nullptr) {
        void *itemData = *reinterpret_cast<void **>((uintptr_t) instance + kOffShopUIItemItemData);
        PatchShopUIItemDataFields(itemData);
    }

    if (old_ShopUIItem_BuyDailyItem != nullptr) {
        old_ShopUIItem_BuyDailyItem(instance);
    }

    ApplyStaminaPatch();
}

void ShopUIItem_BuyShopItem(void *instance) {
    if (instance != nullptr) {
        void *itemData = *reinterpret_cast<void **>((uintptr_t) instance + kOffShopUIItemItemData);
        PatchShopUIItemDataFields(itemData);
    }

    if (old_ShopUIItem_BuyShopItem != nullptr) {
        old_ShopUIItem_BuyShopItem(instance);
    }

    ApplyStaminaPatch();
}

bool RedNotiInfo_IsPossibleGoldShop(void *instance) {
    if (g_unlockShopEventTabs) {
        return true;
    }
    if (old_RedNotiInfo_IsPossibleGoldShop != nullptr) {
        return old_RedNotiInfo_IsPossibleGoldShop(instance);
    }
    return false;
}

bool RedNotiInfo_IsPossibleDailyShop(void *instance) {
    if (g_unlockShopEventTabs) {
        return true;
    }
    if (old_RedNotiInfo_IsPossibleDailyShop != nullptr) {
        return old_RedNotiInfo_IsPossibleDailyShop(instance);
    }
    return false;
}

bool NetDataUtil_IsPossiblePlayWorldTour() {
    if (g_unlockShopEventTabs) {
        return true;
    }
    if (old_NetDataUtil_IsPossiblePlayWorldTour != nullptr) {
        return old_NetDataUtil_IsPossiblePlayWorldTour();
    }
    return false;
}

bool NetDataUtil_IsPossibleWorldTourShop() {
    if (g_unlockShopEventTabs) {
        return true;
    }
    if (old_NetDataUtil_IsPossibleWorldTourShop != nullptr) {
        return old_NetDataUtil_IsPossibleWorldTourShop();
    }
    return false;
}

void *GameManager_CoFinishCostumeMatch(void *instance, bool isWin) {
    if (g_beachMatchVictoryReward) {
        isWin = true;
    }

    void *result = nullptr;
    if (old_GameManager_CoFinishCostumeMatch != nullptr) {
        result = old_GameManager_CoFinishCostumeMatch(instance, isWin);
    }

    ApplyStaminaPatch();
    return result;
}

void LeaguePopup_OnLeagueStartClick(void *instance) {
    ApplyStaminaPatch();

    if (old_LeaguePopup_OnLeagueStartClick != nullptr) {
        old_LeaguePopup_OnLeagueStartClick(instance);
    }

    ApplyStaminaPatch();
}

void CostumeMatchPopup_OnStartClick(void *instance) {
    ApplyStaminaPatch();

    if (old_CostumeMatchPopup_OnStartClick != nullptr) {
        old_CostumeMatchPopup_OnStartClick(instance);
    }

    ApplyStaminaPatch();
}

bool AdsManager_CheckInterstitialAdsStartCondition(void *instance) {
    if (g_removeAds) {
        return false;
    }

    if (old_AdsManager_CheckInterstitialAdsStartCondition != nullptr) {
        return old_AdsManager_CheckInterstitialAdsStartCondition(instance);
    }
    return false;
}

void AdsManager_ShowInterstitial(void *instance) {
    if (g_removeAds) {
        return;
    }

    if (old_AdsManager_ShowInterstitial != nullptr) {
        old_AdsManager_ShowInterstitial(instance);
    }
}

void AdsManager_StartInterstitialAd(void *instance) {
    if (g_removeAds) {
        return;
    }

    if (old_AdsManager_StartInterstitialAd != nullptr) {
        old_AdsManager_StartInterstitialAd(instance);
    }
}

void Hooks() {
    // Unlock Limited Characters + shop zero-cost path.
    HOOK("0x2445F10", NetDataUtil_CanBuy, old_NetDataUtil_CanBuy); // NetDataUtil.CanBuy
    HOOK("0x25031E8", ShopUIItem_UpdateData, old_ShopUIItem_UpdateData); // ShopUIItem.UpdateData
    HOOK("0x250A620", ShopUIItem_BuyDailyItem, old_ShopUIItem_BuyDailyItem); // ShopUIItem.BuyDailyItem
    HOOK("0x2509C6C", ShopUIItem_BuyShopItem, old_ShopUIItem_BuyShopItem); // ShopUIItem.BuyShopItem

    // Unlock Shop/Event tabs.
    HOOK("0x24F2AA8", RedNotiInfo_IsPossibleGoldShop, old_RedNotiInfo_IsPossibleGoldShop); // RedNotiInfo.IsPossibleGoldShop
    HOOK("0x24F2828", RedNotiInfo_IsPossibleDailyShop, old_RedNotiInfo_IsPossibleDailyShop); // RedNotiInfo.IsPossibleDailyShop
    HOOK("0x244CB4C", NetDataUtil_IsPossiblePlayWorldTour, old_NetDataUtil_IsPossiblePlayWorldTour); // NetDataUtil.IsPossiblePlayWorldTour
    HOOK("0x244CC28", NetDataUtil_IsPossibleWorldTourShop, old_NetDataUtil_IsPossibleWorldTourShop); // NetDataUtil.IsPossibleWorldTourShop

    // Beach match reward/stamina path.
    HOOK("0x25305FC", GameManager_CoFinishCostumeMatch, old_GameManager_CoFinishCostumeMatch); // GameManager.CoFinishCostumeMatch
    HOOK("0x23B7BC4", LeaguePopup_OnLeagueStartClick, old_LeaguePopup_OnLeagueStartClick); // LeaguePopup.OnLeagueStartClick
    HOOK("0x23A8714", CostumeMatchPopup_OnStartClick, old_CostumeMatchPopup_OnStartClick); // CostumeMatchPopup.OnStartClick

    // Remove ads.
    HOOK("0x250DE58", AdsManager_CheckInterstitialAdsStartCondition, old_AdsManager_CheckInterstitialAdsStartCondition); // AdsManager.CheckInterstitialAdsStartCondition
    HOOK("0x250DEF8", AdsManager_ShowInterstitial, old_AdsManager_ShowInterstitial); // AdsManager.ShowInterstitial
    HOOK("0x250D8BC", AdsManager_StartInterstitialAd, old_AdsManager_StartInterstitialAd); // AdsManager.StartInterstitialAd

    LOGI("%s", (const char *) OBFUSCATE("VolleyGirls requested hooks installed"));
}

#endif //ZYCHEATS_SGUYS_FUNCTIONS_H
