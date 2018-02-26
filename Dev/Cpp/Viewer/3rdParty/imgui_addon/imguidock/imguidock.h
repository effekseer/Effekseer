#ifndef IMGUIDOCK_H_
#define IMGUIDOCK_H_

// based on https://github.com/nem0/LumixEngine/blob/master/external/imgui/imgui_dock.h
// Lumix Engine Dock. From: https://github.com/nem0/LumixEngine/blob/master/src/editor/imgui/imgui_dock.h
/*
The MIT License (MIT)

Copyright (c) 2013-2016 Mikulas Florek

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
// modified from https://bitbucket.org/duangle/liminal/src/tip/src/liminal/imgui_dock.h

// USAGE:
/*
        // Outside any ImGuiWindow:

        // Windowed:
        if (ImGui::Begin("imguidock window (= lumix engine's dock system)",NULL,ImVec2(500, 500),0.95f,ImGuiWindowFlags_NoScrollbar)) {
            ImGui::BeginDockspace();
            static char tmp[128];
            for (int i=0;i<10;i++)  {
                sprintf(tmp,"Dock %d",i);
                if (i==9) ImGui::SetNextDock(ImGuiDockSlot_Bottom);// optional
                if(ImGui::BeginDock(tmp))  {
                    ImGui::Text("Content of dock window %d goes here",i);
                }
                ImGui::EndDock();
            }
            ImGui::EndDockspace();
        }
        ImGui::End();


        // Fullscreen (without visual artifacts):
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        const ImGuiWindowFlags flags =  (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
        const float oldWindowRounding = ImGui::GetStyle().WindowRounding;ImGui::GetStyle().WindowRounding = 0;
        const bool visible = ImGui::Begin("imguidock window (= lumix engine's dock system)",NULL,ImVec2(0, 0),1.0f,flags);
        ImGui::GetStyle().WindowRounding = oldWindowRounding;
        if (visible) {
            ImGui::BeginDockspace();
            static char tmp[128];
            for (int i=0;i<10;i++)  {
                sprintf(tmp,"Dock %d",i);
                if (i==9) ImGui::SetNextDock(ImGuiDockSlot_Bottom);// optional
                if(ImGui::BeginDock(tmp))  {
                    ImGui::Text("Content of dock window %d goes here",i);
                }
                ImGui::EndDock();
            }
            ImGui::EndDockspace();
        }
        ImGui::End();
*/

#ifndef IMGUI_API
#include <imgui.h>
#endif //IMGUI_API
 
typedef enum ImGuiDockSlot {
    ImGuiDockSlot_Left=0,
    ImGuiDockSlot_Right,
    ImGuiDockSlot_Top,
    ImGuiDockSlot_Bottom,
    ImGuiDockSlot_Tab,

    ImGuiDockSlot_Float,
    ImGuiDockSlot_None
} ImGuiDockSlot;

namespace ImGui{

struct DockContext;

// Create, destroy and change dock contexts. These functions are
// entirely optional if you only need one dock context - a default
// dock context is created on startup and set by default.

// Each created context must be destroyed using DestroyDockContext.
IMGUI_API DockContext* CreateDockContext();

// Destroying the default dock context is a no-op.
IMGUI_API void DestroyDockContext(DockContext* dock);

// SetCurrentDockContext(NULL) will select the default dock context.
IMGUI_API void SetCurrentDockContext(DockContext* dock);

IMGUI_API DockContext* GetCurrentDockContext();

IMGUI_API void BeginDockspace();
IMGUI_API void EndDockspace();
IMGUI_API void ShutdownDock();
IMGUI_API void SetNextDock(ImGuiDockSlot slot);
// 'default_size', when positive, will be used as the initial size of the Window when in floating/undocked mode.
// When the floating/undocked window is manually resized, the last modified window size is kept (and the passed argument is ignored).
// If 'default_size' is negative, any manual resizing (of the floating window) will be lost when the window is re-docked.
// Please note that if you LoadDock(...) the last saved value will be used (so 'default_size' can still be ignored).
IMGUI_API bool BeginDock(const char* label, bool* opened = NULL, ImGuiWindowFlags extra_flags = 0, const ImVec2& default_size = ImVec2(0,0));
IMGUI_API void EndDock();
IMGUI_API void SetDockActive();
IMGUI_API void DockDebugWindow();

// Ported from the original "Lua binding" code
#if (defined(IMGUIHELPER_H_) && !defined(NO_IMGUIHELPER_SERIALIZATION))
#   ifndef NO_IMGUIHELPER_SERIALIZATION_SAVE
    IMGUI_API bool SaveDock(ImGuiHelper::Serializer& s);
    IMGUI_API bool SaveDock(const char* filename);
#   endif //NO_IMGUIHELPER_SERIALIZATION_SAVE
#   ifndef NO_IMGUIHELPER_SERIALIZATION_LOAD
    IMGUI_API bool LoadDock(ImGuiHelper::Deserializer& d,const char ** pOptionalBufferStart=NULL);
    IMGUI_API bool LoadDock(const char* filename);
#   endif //NO_IMGUIHELPER_SERIALIZATION_LOAD
#endif //(defined(IMGUIHELPER_H_) && !defined(NO_IMGUIHELPER_SERIALIZATION))

} // namespace ImGui

extern bool gImGuiDockReuseTabWindowTextureIfAvailable; // [true] (used only when available)


#endif //IMGUIDOCK_H_

