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
            if (BeginTabItem(OBFUSCATE("Shop"))) {
                Checkbox(OBFUSCATE("Unlock Limited Characters"), &g_unlockLimitedCharacters);
                PushTextWrapPos(0.0f);
                TextUnformatted(OBFUSCATE("Sets all shop item prices to 0 and unlocks them unconditionally."));
                PopTextWrapPos();

                Separator();

                Checkbox(OBFUSCATE("Unlock Shop/Event Tabs"), &g_unlockShopEventTabs);
                EndTabItem();
            }

            if (BeginTabItem(OBFUSCATE("Ads"))) {
                Checkbox(OBFUSCATE("Ads Removal With Rewards"), &g_removeAds);
                PushTextWrapPos(0.0f);
                TextUnformatted(OBFUSCATE("Disables interstitial start checks & sets VideoIsLoaded true for rewards."));
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
                Bullet();
                SameLine();
                TextUnformatted(OBFUSCATE("Strictly ported from Lua toolboxes."));
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
