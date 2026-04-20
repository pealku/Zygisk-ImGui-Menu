//
// Created by lbert on 4/15/2023.
//

#ifndef ZYGISK_MENU_TEMPLATE_MENU_H
#define ZYGISK_MENU_TEMPLATE_MENU_H

using namespace ImGui;

void DrawMenu()
{
    {
        Begin(OBFUSCATE("VolleyGirls Mod Menu"));
        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_FittingPolicyResizeDown;
        if (BeginTabBar(OBFUSCATE("Main Tab"), tab_bar_flags)) {
            if (BeginTabItem(OBFUSCATE("Character"))) {
                PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.45f, 1.0f));
                TextUnformatted(OBFUSCATE("Anti-Cheat Bypass Active"));
                PopStyleColor();
                Checkbox(OBFUSCATE("Show All Character Cards"), &g_showAllCharacterCards);
                PushTextWrapPos(0.0f);
                TextUnformatted(OBFUSCATE("Enable before opening Character Popup. Tapping a character card will force select and arrange, including locked entries."));
                PopTextWrapPos();
                EndTabItem();
            }
            if (BeginTabItem(OBFUSCATE("Shop/Event"))) {
                Checkbox(OBFUSCATE("Unlock Limited Characters"), &g_unlockLimitedCharacters);
                Checkbox(OBFUSCATE("Unlock Shop/Event Tabs"), &g_unlockShopEventTabs);
                PushTextWrapPos(0.0f);
                TextUnformatted(OBFUSCATE("Limited character unlock patches CanBuy and shop item fields. Shop/Event toggle unlocks Daily/Gold shop and WorldTour tab checks."));
                PopTextWrapPos();
                EndTabItem();
            }
            if (BeginTabItem(OBFUSCATE("Player"))) {
                Checkbox(OBFUSCATE("No Stamina Decrease"), &g_noStaminaDecrease);
                SliderInt(OBFUSCATE("Locked Stamina Value"), &g_staminaLockValue, 1, 9999);
                PushTextWrapPos(0.0f);
                TextUnformatted(OBFUSCATE("Uses NetDataUtil.get_UserData + UserData.st offset from dump.cs and restores stamina while enabled."));
                PopTextWrapPos();
                EndTabItem();
            }
            if (BeginTabItem(OBFUSCATE("Info"))) {
                Bullet();
                SameLine();
                TextUnformatted(OBFUSCATE("Target package: com.daerigame.volleygirls"));
                Bullet();
                SameLine();
                TextUnformatted(OBFUSCATE("Target library: libil2cpp.so"));
                EndTabItem();
            }
            EndTabBar();
        }
        Patches();
        End();
    }
}

void SetupImgui() {
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO &io = GetIO();
    io.DisplaySize = ImVec2((float) glWidth, (float) glHeight);
    ImGui_ImplOpenGL3_Init("#version 100");
    StyleColorsDark();
    GetStyle().ScaleAllSizes(7.0f);
    io.Fonts->AddFontFromMemoryTTF(Roboto_Regular, 30, 30.0f);
}


EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);

    if (!setupimg)
    {
        SetupImgui();
        setupimg = true;
    }

    ImGuiIO &io = GetIO();
    ImGui_ImplOpenGL3_NewFrame();
    NewFrame();

    DrawMenu();

    EndFrame();
    Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return old_eglSwapBuffers(dpy, surface);
}

#endif //ZYGISK_MENU_TEMPLATE_MENU_H
