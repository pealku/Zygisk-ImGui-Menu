#ifndef ZYCHEATS_SGUYS_FUNCTIONS_H
#define ZYCHEATS_SGUYS_FUNCTIONS_H

#include <cstdint>

// Menu state
bool g_unlockAllCharacters = false;

// CharacterPopup internals and card item data offsets from dump.cs.
constexpr uintptr_t kOffCardUIItemData = 0x108;
constexpr uintptr_t kOffCardUIItemDataCharacterData = 0x10;
constexpr uintptr_t kOffCardUIItemDataCharacterPopup = 0x20;
constexpr uintptr_t kOffCharacterDataId = 0x10;

// Direct call targets (RVA from dump.cs).
constexpr uintptr_t kOffCharacterPopupSetSelectedCharacterId = 0x239E44C;
constexpr uintptr_t kOffCharacterPopupRefreshDetails = 0x239EEC8;
constexpr uintptr_t kOffCharacterPopupOnMainArrangeButtonClick = 0x23A385C;

void *(*CharacterPopup_SetSelectedCharacterId)(void *instance, void *characterId) = nullptr;
void (*CharacterPopup_RefreshDetails)(void *instance, int directionType) = nullptr;
void (*CharacterPopup_OnMainArrangeButtonClick)(void *instance) = nullptr;

// RefreshScroll predicates used by CharacterPopup card list filtering.
bool (*old_RefreshScroll_Filter2)(void *instance, void *entry) = nullptr;
bool (*old_RefreshScroll_Filter3)(void *instance, void *entry) = nullptr;
bool (*old_RefreshScroll_Filter4)(void *instance, void *entry) = nullptr;
bool (*old_RefreshScroll_Filter8)(void *instance, void *entry) = nullptr;
bool (*old_RefreshScroll_Filter9)(void *instance, void *entry) = nullptr;

// Card click hook.
void (*old_CardUIItem_OnCardClick)(void *instance) = nullptr;

// Anti-cheat hooks.
void (*old_CheatScannerSample_Update)(void *instance) = nullptr;
void (*old_SpeedHackScanner_Update)(void *instance) = nullptr;
bool (*old_SpeedHack_IsDetect)() = nullptr;
bool (*old_GPrestoEngine_IsDetect)() = nullptr;
bool (*old_GPrestoAOS_SendDetectionLog)(void *instance, int32_t type_code) = nullptr;

void Pointers() {
    CharacterPopup_SetSelectedCharacterId = reinterpret_cast<void *(*)(void *, void *)>(
            g_il2cppBaseMap.startAddress + kOffCharacterPopupSetSelectedCharacterId);
    CharacterPopup_RefreshDetails = reinterpret_cast<void (*)(void *, int)>(
            g_il2cppBaseMap.startAddress + kOffCharacterPopupRefreshDetails);
    CharacterPopup_OnMainArrangeButtonClick = reinterpret_cast<void (*)(void *)>(
            g_il2cppBaseMap.startAddress + kOffCharacterPopupOnMainArrangeButtonClick);
}

void Patches() {
    // Hook-driven feature set; no raw byte patches required for this import.
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

bool RefreshScroll_Filter2(void *instance, void *entry) {
    if (g_unlockAllCharacters) {
        return true;
    }
    return old_RefreshScroll_Filter2(instance, entry);
}

bool RefreshScroll_Filter3(void *instance, void *entry) {
    if (g_unlockAllCharacters) {
        return true;
    }
    return old_RefreshScroll_Filter3(instance, entry);
}

bool RefreshScroll_Filter4(void *instance, void *entry) {
    if (g_unlockAllCharacters) {
        return true;
    }
    return old_RefreshScroll_Filter4(instance, entry);
}

bool RefreshScroll_Filter8(void *instance, void *entry) {
    if (g_unlockAllCharacters) {
        return true;
    }
    return old_RefreshScroll_Filter8(instance, entry);
}

bool RefreshScroll_Filter9(void *instance, void *entry) {
    if (g_unlockAllCharacters) {
        return true;
    }
    return old_RefreshScroll_Filter9(instance, entry);
}

void CardUIItem_OnCardClick(void *instance) {
    if (!g_unlockAllCharacters || instance == nullptr ||
        CharacterPopup_SetSelectedCharacterId == nullptr ||
        CharacterPopup_RefreshDetails == nullptr) {
        old_CardUIItem_OnCardClick(instance);
        return;
    }

    void *itemData = *(void **)((uintptr_t) instance + kOffCardUIItemData);
    if (itemData == nullptr) {
        old_CardUIItem_OnCardClick(instance);
        return;
    }

    void *characterData = *(void **)((uintptr_t) itemData + kOffCardUIItemDataCharacterData);
    void *characterPopup = *(void **)((uintptr_t) itemData + kOffCardUIItemDataCharacterPopup);
    if (characterData == nullptr || characterPopup == nullptr) {
        old_CardUIItem_OnCardClick(instance);
        return;
    }

    void *characterId = *(void **)((uintptr_t) characterData + kOffCharacterDataId);
    if (characterId == nullptr) {
        old_CardUIItem_OnCardClick(instance);
        return;
    }

    CharacterPopup_SetSelectedCharacterId(characterPopup, characterId);
    CharacterPopup_RefreshDetails(characterPopup, 0);

    if (CharacterPopup_OnMainArrangeButtonClick != nullptr) {
        CharacterPopup_OnMainArrangeButtonClick(characterPopup);
    }
}

void Hooks() {
    // Character list ownership filters inside CharacterPopup.RefreshScroll.
    HOOK("0x23A619C", RefreshScroll_Filter2, old_RefreshScroll_Filter2); // <>c__DisplayClass126_0.<RefreshScroll>b__2
    HOOK("0x23A61FC", RefreshScroll_Filter3, old_RefreshScroll_Filter3); // <>c__DisplayClass126_0.<RefreshScroll>b__3
    HOOK("0x23A629C", RefreshScroll_Filter4, old_RefreshScroll_Filter4); // <>c__DisplayClass126_0.<RefreshScroll>b__4
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

    LOGI("%s", (const char *) OBFUSCATE("VolleyGirls character-unlock and anti-cheat hooks installed"));
}

#endif //ZYCHEATS_SGUYS_FUNCTIONS_H
