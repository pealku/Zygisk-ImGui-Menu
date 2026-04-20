#ifndef ZYCHEATS_SGUYS_FUNCTIONS_H
#define ZYCHEATS_SGUYS_FUNCTIONS_H

#include <cstdint>

// Menu state
bool g_unlockLimitedCharacters = false;
bool g_unlockShopEventTabs = false;
bool g_showAllCharacterCards = false;
bool g_noStaminaDecrease = false;
int g_staminaLockValue = 9999;

// CharacterPopup internals and card item data offsets from dump.cs.
constexpr uintptr_t kOffCardUIItemData = 0x108;
constexpr uintptr_t kOffCardUIItemDataCharacterData = 0x10;
constexpr uintptr_t kOffCardUIItemDataCharacterPopup = 0x20;
constexpr uintptr_t kOffCharacterDataId = 0x10;
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

// UserData field offset from dump.cs.
constexpr uintptr_t kOffUserDataStamina = 0x16C;

// Direct call targets (RVA from dump.cs).
constexpr uintptr_t kOffCharacterPopupSetSelectedCharacterId = 0x239E44C;
constexpr uintptr_t kOffCharacterPopupRefreshDetails = 0x239EEC8;
constexpr uintptr_t kOffCharacterPopupOnMainArrangeButtonClick = 0x23A385C;
constexpr uintptr_t kOffNetDataUtilGetUserData = 0x2432E1C;

void *(*CharacterPopup_SetSelectedCharacterId)(void *instance, void *characterId) = nullptr;
void (*CharacterPopup_RefreshDetails)(void *instance, int directionType) = nullptr;
void (*CharacterPopup_OnMainArrangeButtonClick)(void *instance) = nullptr;
void *(*NetDataUtil_get_UserData)() = nullptr;

// RefreshScroll predicates used by CharacterPopup card list filtering.
bool (*old_RefreshScroll_Filter2)(void *instance, void *entry) = nullptr;
bool (*old_RefreshScroll_Filter3)(void *instance, void *entry) = nullptr;
bool (*old_RefreshScroll_Filter4)(void *instance, void *entry) = nullptr;
bool (*old_RefreshScroll_Filter7)(void *instance, void *entry) = nullptr;
bool (*old_RefreshScroll_Filter8)(void *instance, void *entry) = nullptr;
bool (*old_RefreshScroll_Filter9)(void *instance, void *entry) = nullptr;

// Card click hook.
void (*old_CardUIItem_OnCardClick)(void *instance) = nullptr;

// Shop / tab hooks.
bool (*old_NetDataUtil_CanBuy)(void *shopData) = nullptr;
void (*old_ShopUIItem_UpdateData)(void *instance, void *scrollData) = nullptr;
bool (*old_RedNotiInfo_IsPossibleGoldShop)(void *instance) = nullptr;
bool (*old_RedNotiInfo_IsPossibleDailyShop)(void *instance) = nullptr;
bool (*old_NetDataUtil_IsPossiblePlayWorldTour)() = nullptr;
bool (*old_NetDataUtil_IsPossibleWorldTourShop)() = nullptr;

// Anti-cheat hooks.
void (*old_CheatScannerSample_Update)(void *instance) = nullptr;
void (*old_SpeedHackScanner_Update)(void *instance) = nullptr;
bool (*old_SpeedHack_IsDetect)() = nullptr;
bool (*old_GPrestoEngine_IsDetect)() = nullptr;
bool (*old_GPrestoAOS_SendDetectionLog)(void *instance, int32_t type_code) = nullptr;

void PatchShopDataFields(void *shopData) {
    if (shopData == nullptr) {
        return;
    }

    *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataShow) = 1;
    *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataShowAfterBuy) = 1;
    *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataCostAmount) = 0;
    *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataLimitValue) = 0;
    *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataLimitAmount) = 999999;
    *reinterpret_cast<int32_t *>((uintptr_t) shopData + kOffShopDataUnlockValue) = 0;
}

void PatchShopUIItemDataFields(void *itemData) {
    if (itemData == nullptr) {
        return;
    }

    *reinterpret_cast<uint8_t *>((uintptr_t) itemData + kOffShopUIItemDataCanBuy) = 1;
    void *shopData = *reinterpret_cast<void **>((uintptr_t) itemData + kOffShopUIItemDataShopData);
    PatchShopDataFields(shopData);
}

void ApplyStaminaLock() {
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

    int32_t *stamina = reinterpret_cast<int32_t *>((uintptr_t) userData + kOffUserDataStamina);
    if (*stamina < g_staminaLockValue) {
        *stamina = g_staminaLockValue;
    }
}

void Pointers() {
    CharacterPopup_SetSelectedCharacterId = reinterpret_cast<void *(*)(void *, void *)>(
            g_il2cppBaseMap.startAddress + kOffCharacterPopupSetSelectedCharacterId);
    CharacterPopup_RefreshDetails = reinterpret_cast<void (*)(void *, int)>(
            g_il2cppBaseMap.startAddress + kOffCharacterPopupRefreshDetails);
    CharacterPopup_OnMainArrangeButtonClick = reinterpret_cast<void (*)(void *)>(
            g_il2cppBaseMap.startAddress + kOffCharacterPopupOnMainArrangeButtonClick);
    NetDataUtil_get_UserData = reinterpret_cast<void *(*)()>(
            g_il2cppBaseMap.startAddress + kOffNetDataUtilGetUserData);
}

void Patches() {
    ApplyStaminaLock();
}

void CheatScannerSample_Update(void *instance) {
    // Block update logic by swallowing execution.
}

void SpeedHackScanner_Update(void *instance) {
    // Block update logic by swallowing execution.
}

bool SpeedHack_IsDetect() {
    return false;
}

bool GPrestoEngine_IsDetect() {
    return false;
}

bool GPrestoAOS_SendDetectionLog(void *instance, int32_t type_code) {
    return false;
}

bool NetDataUtil_CanBuy(void *shopData) {
    if (g_unlockLimitedCharacters) {
        PatchShopDataFields(shopData);
        return true;
    }
    if (old_NetDataUtil_CanBuy != nullptr) {
        return old_NetDataUtil_CanBuy(shopData);
    }
    return false;
}

void ShopUIItem_UpdateData(void *instance, void *scrollData) {
    if (g_unlockLimitedCharacters && instance != nullptr) {
        void *itemData = *reinterpret_cast<void **>((uintptr_t) instance + kOffShopUIItemItemData);
        PatchShopUIItemDataFields(itemData);
    }

    if (old_ShopUIItem_UpdateData != nullptr) {
        old_ShopUIItem_UpdateData(instance, scrollData);
    }

    if (g_unlockLimitedCharacters && instance != nullptr) {
        void *itemData = *reinterpret_cast<void **>((uintptr_t) instance + kOffShopUIItemItemData);
        PatchShopUIItemDataFields(itemData);
    }
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

bool RefreshScroll_Filter2(void *instance, void *entry) {
    if (g_showAllCharacterCards) {
        return true;
    }
    if (old_RefreshScroll_Filter2 != nullptr) {
        return old_RefreshScroll_Filter2(instance, entry);
    }
    return false;
}

bool RefreshScroll_Filter3(void *instance, void *entry) {
    if (g_showAllCharacterCards) {
        return true;
    }
    if (old_RefreshScroll_Filter3 != nullptr) {
        return old_RefreshScroll_Filter3(instance, entry);
    }
    return false;
}

bool RefreshScroll_Filter4(void *instance, void *entry) {
    if (g_showAllCharacterCards) {
        return true;
    }
    if (old_RefreshScroll_Filter4 != nullptr) {
        return old_RefreshScroll_Filter4(instance, entry);
    }
    return false;
}

bool RefreshScroll_Filter7(void *instance, void *entry) {
    if (g_showAllCharacterCards) {
        return true;
    }
    if (old_RefreshScroll_Filter7 != nullptr) {
        return old_RefreshScroll_Filter7(instance, entry);
    }
    return false;
}

bool RefreshScroll_Filter8(void *instance, void *entry) {
    if (g_showAllCharacterCards) {
        return true;
    }
    if (old_RefreshScroll_Filter8 != nullptr) {
        return old_RefreshScroll_Filter8(instance, entry);
    }
    return false;
}

bool RefreshScroll_Filter9(void *instance, void *entry) {
    if (g_showAllCharacterCards) {
        return true;
    }
    if (old_RefreshScroll_Filter9 != nullptr) {
        return old_RefreshScroll_Filter9(instance, entry);
    }
    return false;
}

void CardUIItem_OnCardClick(void *instance) {
    if (!g_showAllCharacterCards || instance == nullptr ||
        CharacterPopup_SetSelectedCharacterId == nullptr ||
        CharacterPopup_RefreshDetails == nullptr) {
        if (old_CardUIItem_OnCardClick != nullptr) {
            old_CardUIItem_OnCardClick(instance);
        }
        return;
    }

    void *itemData = *(void **)((uintptr_t) instance + kOffCardUIItemData);
    if (itemData == nullptr) {
        if (old_CardUIItem_OnCardClick != nullptr) {
            old_CardUIItem_OnCardClick(instance);
        }
        return;
    }

    void *characterData = *(void **)((uintptr_t) itemData + kOffCardUIItemDataCharacterData);
    void *characterPopup = *(void **)((uintptr_t) itemData + kOffCardUIItemDataCharacterPopup);
    if (characterData == nullptr || characterPopup == nullptr) {
        if (old_CardUIItem_OnCardClick != nullptr) {
            old_CardUIItem_OnCardClick(instance);
        }
        return;
    }

    void *characterId = *(void **)((uintptr_t) characterData + kOffCharacterDataId);
    if (characterId == nullptr) {
        if (old_CardUIItem_OnCardClick != nullptr) {
            old_CardUIItem_OnCardClick(instance);
        }
        return;
    }

    CharacterPopup_SetSelectedCharacterId(characterPopup, characterId);
    CharacterPopup_RefreshDetails(characterPopup, 0);

    if (CharacterPopup_OnMainArrangeButtonClick != nullptr) {
        CharacterPopup_OnMainArrangeButtonClick(characterPopup);
    }
}

void Hooks() {
    // Unlock Limited Characters (Lua: NetDataUtil.CanBuy + ShopData/ShopUIItemData fields).
    HOOK("0x2445F10", NetDataUtil_CanBuy, old_NetDataUtil_CanBuy); // NetDataUtil.CanBuy
    HOOK("0x25031E8", ShopUIItem_UpdateData, old_ShopUIItem_UpdateData); // ShopUIItem.UpdateData

    // Unlock Shop/Event Tabs (Lua parity).
    HOOK("0x24F2AA8", RedNotiInfo_IsPossibleGoldShop, old_RedNotiInfo_IsPossibleGoldShop); // RedNotiInfo.IsPossibleGoldShop
    HOOK("0x24F2828", RedNotiInfo_IsPossibleDailyShop, old_RedNotiInfo_IsPossibleDailyShop); // RedNotiInfo.IsPossibleDailyShop
    HOOK("0x244CB4C", NetDataUtil_IsPossiblePlayWorldTour, old_NetDataUtil_IsPossiblePlayWorldTour); // NetDataUtil.IsPossiblePlayWorldTour
    HOOK("0x244CC28", NetDataUtil_IsPossibleWorldTourShop, old_NetDataUtil_IsPossibleWorldTourShop); // NetDataUtil.IsPossibleWorldTourShop

    // Character list ownership filters inside CharacterPopup.RefreshScroll.
    HOOK("0x23A619C", RefreshScroll_Filter2, old_RefreshScroll_Filter2); // <>c__DisplayClass126_0.<RefreshScroll>b__2
    HOOK("0x23A61FC", RefreshScroll_Filter3, old_RefreshScroll_Filter3); // <>c__DisplayClass126_0.<RefreshScroll>b__3
    HOOK("0x23A629C", RefreshScroll_Filter4, old_RefreshScroll_Filter4); // <>c__DisplayClass126_0.<RefreshScroll>b__4
    HOOK("0x23A630C", RefreshScroll_Filter7, old_RefreshScroll_Filter7); // <>c__DisplayClass126_1.<RefreshScroll>b__7
    HOOK("0x23A6074", RefreshScroll_Filter8, old_RefreshScroll_Filter8); // <>c.<RefreshScroll>b__126_8
    HOOK("0x23A6364", RefreshScroll_Filter9, old_RefreshScroll_Filter9); // <>c__DisplayClass126_2.<RefreshScroll>b__9

    // Card click path for selecting and arranging locked cards.
    HOOK("0x24F9638", CardUIItem_OnCardClick, old_CardUIItem_OnCardClick); // CardUIItem.OnCardClick

    // Apply anti-cheat detours.
    HOOK("0x22FBCA0", CheatScannerSample_Update, old_CheatScannerSample_Update); // CheatScannerSample.Update
    HOOK("0x22FC680", SpeedHackScanner_Update, old_SpeedHackScanner_Update); // SpeedHackScanner.Update
    HOOK("0x3F20838", SpeedHack_IsDetect, old_SpeedHack_IsDetect); // SpeedHack.IsDetect
    HOOK("0x3F30EE8", GPrestoEngine_IsDetect, old_GPrestoEngine_IsDetect); // GPrestoEngine.IsDetect
    HOOK("0x3F2F614", GPrestoAOS_SendDetectionLog, old_GPrestoAOS_SendDetectionLog); // GPrestoAOS.SendDetectionLog

    LOGI("%s", (const char *) OBFUSCATE("VolleyGirls toolbox features + anti-cheat hooks installed"));
}

#endif //ZYCHEATS_SGUYS_FUNCTIONS_H
