//----------------------------------------------------------------------------------
// File:        UIRenderer.h
// SDK Version: 2.0
// Email:       StreamlineSupport@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------
#pragma once

#include "StreamlineSample.h"
#include "SLWrapper.h"
#include "UIData.h"

#include <donut/app/DeviceManager.h>
#include <imgui_internal.h>
#include <donut/engine/Scene.h>
#include <nvrhi/nvrhi.h>
#include <donut/app/imgui_renderer.h>

void pushDisabled() {
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
}
void popDisabled() {
    ImGui::PopItemFlag();
    ImGui::PopStyleVar();
}

constexpr ImU32 TITLE_COL = IM_COL32(0, 255, 0, 255);

class UIRenderer : public ImGui_Renderer
{
private:
    std::shared_ptr<StreamlineSample> m_app;
    ImFont* m_font_small = nullptr;
    ImFont* m_font_medium = nullptr;
    ImFont* m_font_large = nullptr;
    UIData& m_ui;

    bool    m_dev_view = false;
    int     m_dev_view_dlss_mode = 0;
    int     m_dev_view_TopLevelDLSS = 1;


public:
    UIRenderer(DeviceManager* deviceManager, std::shared_ptr<StreamlineSample> app, UIData& ui)
        : ImGui_Renderer(deviceManager)
        , m_app(app)
        , m_ui(ui) {

        m_font_small = this->LoadFont(*(app->GetRootFs()), "/media/fonts/DroidSans/DroidSans-Mono.ttf", 14.f);
        m_font_medium = this->LoadFont(*(app->GetRootFs()), "/media/fonts/DroidSans/DroidSans-Mono.ttf", 25.f);
        m_font_large = this->LoadFont(*(app->GetRootFs()), "/media/fonts/DroidSans/DroidSans-Mono.ttf", 150.f);

        // IMGUI by default writes in srgb colorSpace, but our back buffer is in rgb, we will simply pre-empt this by gamma shifting the colors.
        auto invGamma = 1.f / 2.2f;
        for (auto i = 0; i < ImGuiCol_COUNT; i++) {
            ImGui::GetStyle().Colors[i].x = powf(ImGui::GetStyle().Colors[i].x, invGamma);
            ImGui::GetStyle().Colors[i].y = powf(ImGui::GetStyle().Colors[i].y, invGamma);
            ImGui::GetStyle().Colors[i].z = powf(ImGui::GetStyle().Colors[i].z, invGamma);
            ImGui::GetStyle().Colors[i].w = powf(ImGui::GetStyle().Colors[i].w, invGamma);
        }

        if (m_ui.DLSS_Mode != sl::DLSSMode::eOff) {
            if (m_ui.DLSS_Mode == sl::DLSSMode::eMaxQuality) m_dev_view_dlss_mode = 2;
            else if (m_ui.DLSS_Mode == sl::DLSSMode::eBalanced) m_dev_view_dlss_mode = 3;
            else if (m_ui.DLSS_Mode == sl::DLSSMode::eMaxPerformance) m_dev_view_dlss_mode = 4;
            else if (m_ui.DLSS_Mode == sl::DLSSMode::eUltraPerformance) m_dev_view_dlss_mode = 5;
            else if (m_ui.DLSS_Mode == sl::DLSSMode::eDLAA) m_dev_view_dlss_mode = 6;
        }

    }

protected:
    virtual void buildUI(void) override
    {

        int width, height;
        GetDeviceManager()->GetWindowDimensions(width, height);

        if (m_app->IsSceneLoading())
        {
            BeginFullScreenWindow();

            char messageBuffer[256];
            const auto& stats = Scene::GetLoadingStats();
            snprintf(messageBuffer, std::size(messageBuffer), "Loading scene %s, please wait...\nObjects: %d/%d, Textures: %d/%d",
                m_app->GetCurrentSceneName().c_str(), stats.ObjectsLoaded.load(), stats.ObjectsTotal.load(), m_app->GetTextureCache()->GetNumberOfLoadedTextures(), m_app->GetTextureCache()->GetNumberOfRequestedTextures());

            DrawScreenCenteredText(messageBuffer);

            EndFullScreenWindow();

            return;
        }

        if (m_ui.MouseOverUI) {
            ImGui::SetNextWindowBgAlpha(0.5f);
            ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
            ImGui::SetNextWindowSize(ImVec2(float(width), float(height)));
            ImGui::Begin("BackDrop", 0, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBringToFrontOnFocus);
            ImGui::End();
        }


        ImGui::SetNextWindowPos(ImVec2(width * 0.02f, height * 0.5f), 0, ImVec2(0.f, 0.5f));
        ImGui::SetNextWindowBgAlpha(m_ui.MouseOverUI ? 0.f : 0.2f);
        ImGui::PushFont(m_font_small);
        ImGui::Begin("Settings", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

        if (ImGui::IsAnyItemHovered() || ImGui::IsMouseHoveringRect(ImGui::GetWindowPos(), ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y))) {
            m_ui.MouseOverUI = true;
        }
        else
            m_ui.MouseOverUI = false;

        ImGui::Text("Renderer: %s", GetDeviceManager()->GetRendererString());
        double fps = 1.0 / GetDeviceManager()->GetAverageFrameTimeSeconds();
        ImGui::Text("Engine FPS: %.0f ", fps);
#ifdef DLSSG_ALLOWED // NDA ONLY DLSS-G DLSS_G Release
        if (m_ui.DLSSG_mode == sl::DLSSGMode::eOn ) {
            ImGui::Text("True FPS: %.0f ", m_ui.DLSSG_fps);
        }
#endif // NDA ONLY DLSS-G DLSS_G Release

        // Vsync
#ifdef DLSSG_ALLOWED // NDA ONLY DLSS-G DLSS_G Release
        if (m_ui.DLSSG_mode == sl::DLSSGMode::eOn && !m_dev_view) {
            pushDisabled();
            m_ui.EnableVsync = false;
        }
#endif // NDA ONLY DLSS-G DLSS_G Release
        ImGui::Checkbox("VSync", &m_ui.EnableVsync);
#ifdef DLSSG_ALLOWED // NDA ONLY DLSS-G DLSS_G Release
        if (m_ui.DLSSG_mode == sl::DLSSGMode::eOn && !m_dev_view) {
            popDisabled();
        }
#endif // NDA ONLY DLSS-G DLSS_G Release

        // Resolution 
        std::vector<std::string> Resolutions_strings = { "1280 x 720", "1920 x 1080", "2560 x 1440", "3840 x 2160" };
        std::vector<int2> Resolutions_values = { {1280, 720}, {1920, 1080}, {2560, 1440}, {3840, 2160} };
        int resIndex = -1;
        for (auto i = 0; i < Resolutions_values.size(); ++i) {
            if (Resolutions_values[i].x == m_ui.Resolution.x && Resolutions_values[i].y == m_ui.Resolution.y) {
                resIndex = i;
            }
        }
        if (resIndex == -1) {
            Resolutions_strings.push_back(std::to_string(m_ui.Resolution.x) + " x " + std::to_string(m_ui.Resolution.y) + " (custom)");
            Resolutions_values.push_back(m_ui.Resolution);
            resIndex = (int)Resolutions_strings.size() - 1;
        }
        auto preIndex = resIndex;
        if (ImGui::BeginCombo("Resolution", Resolutions_strings[resIndex].c_str()))
        {
            for (auto i = 0; i < Resolutions_strings.size(); ++i)
            {
                bool is_selected = i == resIndex;
                auto is_selected_pre = i == resIndex;
                if (ImGui::Selectable(Resolutions_strings[i].c_str(), is_selected)) resIndex = i;
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        m_ui.Resolution = Resolutions_values[resIndex];
        if (preIndex != resIndex) {
            m_ui.Resolution_changed = true;
        }

        ImGui::Checkbox("Developer Menu", &m_dev_view);

        if (!m_dev_view) {

            //
            //  Generic DLSS
            //

            ImGui::Separator();

            ImGui::Text("NVIDIA DLSS");
            ImGui::SameLine();
            ImGui::Combo("##DLSSMode", &m_dev_view_TopLevelDLSS, "Off\0On\0");

            ImGui::Indent();
            if (m_dev_view_TopLevelDLSS == 0) {
                pushDisabled();
                m_ui.DLSS_Mode = sl::DLSSMode::eOff;
                m_dev_view_dlss_mode = 0;
#ifdef DLSSG_ALLOWED // NDA ONLY DLSS-G DLSS_G Release
                m_ui.DLSSG_mode = sl::DLSSGMode::eOff;
#endif
                m_ui.REFLEX_Mode = 0;
                m_ui.NIS_Mode = sl::NISMode::eOff;
            }


#ifdef DLSSG_ALLOWED // NDA ONLY DLSS-G DLSS_G Release
            
            //
            //  DLSS Frame Gen
            //

            ImGui::Text("Frame Generation");
            ImGui::SameLine();
            if (!m_ui.DLSSG_Supported || !m_ui.REFLEX_Supported) pushDisabled();
            ImGui::Combo("##FrameGeneration", (int*)&m_ui.DLSSG_mode, "Off\0On\0");
            if (!m_ui.DLSSG_Supported || !m_ui.REFLEX_Supported) popDisabled();
            if (m_ui.DLSSG_status != "") ImGui::Text((std::string("State: ") + m_ui.DLSSG_status).c_str());
#endif // DLSSG_ALLOWED END NDA ONLY DLSS-G DLSS_G Release

            //
            //  DLSS SuperRes
            //

            auto DLSSModeNames = std::vector<std::string>({
                "Off##DLSSModes",
                "Auto##DLSSModes",
                "Quality##DLSSModes",
                "Balanced##DLSSModes",
                "Performance##DLSSModes",
                "UltraPerformance##DLSSModes",
                "DLAA##DLSSModes"
            });

            ImGui::Text("Super Resolution");
            ImGui::SameLine();
            if (!m_ui.DLSS_Supported) pushDisabled();
            if (ImGui::BeginCombo("##SuperRes", DLSSModeNames[m_dev_view_dlss_mode].data()))
            {
                for (int i = 0; i < DLSSModeNames.size(); ++i)
                {
                    bool is_selected = i == m_dev_view_dlss_mode;

                    if (ImGui::Selectable(DLSSModeNames[i].data(), is_selected)) m_dev_view_dlss_mode = i;
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
                if (ImGui::IsItemHovered()) m_ui.MouseOverUI = true;
            }

            if (!m_ui.DLSS_Supported) popDisabled();

            if (m_dev_view_dlss_mode == 0) m_ui.DLSS_Mode = sl::DLSSMode::eOff;
            else if (m_dev_view_dlss_mode == 2) m_ui.DLSS_Mode = sl::DLSSMode::eMaxQuality;
            else if (m_dev_view_dlss_mode == 3) m_ui.DLSS_Mode = sl::DLSSMode::eBalanced;
            else if (m_dev_view_dlss_mode == 4) m_ui.DLSS_Mode = sl::DLSSMode::eMaxPerformance;
            else if (m_dev_view_dlss_mode == 5) m_ui.DLSS_Mode = sl::DLSSMode::eUltraPerformance;
            //else if (m_dev_view_dlss_mode == 6) m_ui.DLSS_Mode = sl::DLSSMode::eUltraQuality;
            else if (m_dev_view_dlss_mode == 6) m_ui.DLSS_Mode = sl::DLSSMode::eDLAA;
            else if (m_dev_view_dlss_mode == 1) {
                if (m_ui.Resolution.x < 1920) m_ui.DLSS_Mode = sl::DLSSMode::eOff;
                else if (m_ui.Resolution.x < 2560) m_ui.DLSS_Mode = sl::DLSSMode::eMaxQuality;
                else if (m_ui.Resolution.x < 3840) m_ui.DLSS_Mode = sl::DLSSMode::eMaxPerformance;
                else m_ui.DLSS_Mode = sl::DLSSMode::eUltraPerformance;
            }

            if (m_ui.DLSS_Mode != sl::DLSSMode::eOff) {
                m_ui.AAMode = AntiAliasingMode::DLSS;
            }
            else {
                m_ui.AAMode = AntiAliasingMode::NONE;
            }

            //
            //  NIS Sharpenning
            //

            ImGui::Text("NIS Sharpenning");
            ImGui::SameLine();
            if (! m_ui.NIS_Supported) pushDisabled();
            int nis_mode = m_ui.NIS_Mode == sl::NISMode::eScaler ? 1 : 0;
            ImGui::Combo("##NISMode", &nis_mode, "Off\0On\0");
            m_ui.NIS_Mode = nis_mode == 1 ? sl::NISMode::eScaler : sl::NISMode::eOff;
            if (! m_ui.NIS_Supported) popDisabled();

            //
            //  Reflex
            //

            ImGui::Text("Nvidia Reflex Low Latency");
            ImGui::SameLine();
            if (!m_ui.REFLEX_Supported) pushDisabled();

#ifdef DLSSG_ALLOWED // NDA ONLY DLSS-G DLSS_G Release
            if (m_ui.DLSSG_mode == sl::DLSSGMode::eOn) {
                auto i = (int)m_ui.REFLEX_Mode - 1;
                i = i < 0 ? 0 : i;
                ImGui::Combo("##Reflex", &i, "On\0On + Boost\0");
                m_ui.REFLEX_Mode = i + 1;
            }
            else
#endif // DLSSG_ALLOWED END NDA ONLY DLSS-G DLSS_G Release
            ImGui::Combo("##Reflex", (int*)&m_ui.REFLEX_Mode, "Off\0On\0On + Boost\0");
            
            if (ImGui::IsItemHovered()) m_ui.MouseOverUI = true;

            if (m_ui.REFLEX_Mode != 0) {
                ImGui::Indent();
                bool useFrameCap = m_ui.REFLEX_CapedFPS != 0;
                ImGui::Checkbox("Reflex FPS Capping", &useFrameCap);
                if (useFrameCap) {
                    if (m_ui.REFLEX_CapedFPS == 0) { m_ui.REFLEX_CapedFPS = 60; }
                    ImGui::SameLine();
                    ImGui::DragInt("##FPSReflexCap", &m_ui.REFLEX_CapedFPS, 1.f, 20, 240);
                }
                else {
                    m_ui.REFLEX_CapedFPS = 0;
                }
                ImGui::Unindent();
            }

            if (!m_ui.REFLEX_Supported) popDisabled();

            if (m_dev_view_TopLevelDLSS == 0) popDisabled();
            ImGui::Unindent();

        }

        else { // if m_dev_view

            //
            // DLSS SuperRes and AA
            //

            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Text, TITLE_COL);
            ImGui::Text("AA and DLSS");
            ImGui::PopStyleColor();

            ImGui::Text("DLSS_Supported: %s", m_ui.DLSS_Supported ? "yes" : "no");

            if (m_ui.DLSS_Supported) {
                ImGui::Combo("AA Mode", (int*)&m_ui.AAMode, "None\0TemporalAA\0DLSS\0");
            }
            else {
                ImGui::Combo("TAA Fallback", (int*)&m_ui.AAMode, "None\0TemporalAA");
            }

            if (m_ui.AAMode == AntiAliasingMode::TEMPORAL) {
                ImGui::Combo("TAA Camera Jitter", (int*)&m_ui.TemporalAntiAliasingJitter, "MSAA\0Halton\0R2\0White Noise\0");
            }


            if (m_ui.AAMode == AntiAliasingMode::DLSS)
            {
                if (m_ui.DLSS_Mode == sl::DLSSMode::eOff) m_ui.DLSS_Mode = sl::DLSSMode::eBalanced;

                // We do not show 'eOff' 
                const char* DLSSModeNames[] = {
                    "Off",
                    "Performance",
                    "Balanced",
                    "Quality",
                    "Ultra-Performance",
                    "Ultra-Quality",
                    "DLAA"
                };

                if (ImGui::BeginCombo("DLSS Mode", DLSSModeNames[(int)m_ui.DLSS_Mode]))
                {
                    for (int i = 0; i < static_cast<int>(sl::DLSSMode::eCount); ++i)
                    {
                        if ((i == static_cast<int>(sl::DLSSMode::eUltraQuality)) || (i == static_cast<int>(sl::DLSSMode::eOff))) continue;

                        bool is_selected = (i == (int)m_ui.DLSS_Mode);

                        if (ImGui::Selectable(DLSSModeNames[i], is_selected)) {
                            m_ui.DLSS_Mode = (sl::DLSSMode)i;
                        }
                        if (is_selected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                auto PresetSlotNames = std::vector<std::string>({
                    "Off##Presets",
                    "MaxPerformance##Presets",
                    "Balanced##Presets",
                    "MaxQuality##Presets",
                    "UltraPerformance##Presets",
                    "UltraQuality##Presets",
                    "DLAA##Presets"
                    });

                auto DLSSPresetNames = std::vector<std::string>({
                    "Default##Presets",
                    "Preset A##Presets",
                    "Preset B##Presets",
                    "Preset C##Presets",
                    "Preset D##Presets",
                    "Preset E##Presets",
                    "Preset F##Presets",
                    });

                if (ImGui::CollapsingHeader("Presets")) {
                    ImGui::Indent();
                    // skip index 0, i.e. OFF
                    for (int j = 0; j < static_cast<int>(sl::DLSSMode::eCount); j++)
                    {

                        if ((j == static_cast<int>(sl::DLSSMode::eUltraQuality)) || (j == static_cast<int>(sl::DLSSMode::eOff))) continue;

                        if (ImGui::BeginCombo(PresetSlotNames[j].c_str(), DLSSPresetNames[static_cast<int>(m_ui.DLSS_presets[j])].data()))
                        {
                            for (int i = 0; i < DLSSPresetNames.size(); ++i) {

                                bool is_selected = i == static_cast<int>(m_ui.DLSS_presets[j]);

                                if (ImGui::Selectable(DLSSPresetNames[i].data(), is_selected)) m_ui.DLSS_presets[j] = (sl::DLSSPreset)i;
                                if (is_selected) ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                            if (ImGui::IsItemHovered()) m_ui.MouseOverUI = true;
                        }
                    }
                    ImGui::Unindent();
                }

                static const char* DLSSResModeNames[] = {
                    "Fixed",
                    "Dynamic"
                };

                if (ImGui::BeginCombo("DLSS Resolution Mode", DLSSResModeNames[(int)m_ui.DLSS_Resolution_Mode]))
                {
                    for (int i = 0; i < (int)RenderingResolutionMode::COUNT; ++i)
                    {
                        bool is_selected = (i == (int)m_ui.DLSS_Resolution_Mode);
                        if (ImGui::Selectable(DLSSResModeNames[i], is_selected)) m_ui.DLSS_Resolution_Mode = (RenderingResolutionMode)i;
                        if (is_selected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                if (m_ui.DLSS_Resolution_Mode == RenderingResolutionMode::DYNAMIC)
                {
                    if (ImGui::Button("Change Res"))
                    {
                        m_ui.DLSS_Dynamic_Res_change = true;
                    }
                }

                ImGui::Checkbox("Debug: Show full input buffer", &m_ui.DLSS_DebugShowFullRenderingBuffer);
                ImGui::Checkbox("Debug: Force Extent use", &m_ui.DLSS_always_use_extents);

                ImGui::Checkbox("Overide LOD Bias", &m_ui.DLSS_lodbias_useoveride);
                if (m_ui.DLSS_lodbias_useoveride) {
                    ImGui::SameLine();
                    ImGui::SliderFloat("", &m_ui.DLSS_lodbias_overide, -2, 2);
                }

            }

            //
            // Reflex
            //

            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Text, TITLE_COL);
            ImGui::Text("Reflex");
            ImGui::PopStyleColor();

            ImGui::Text("Reflex Supported (PCL tracking): %s", m_ui.REFLEX_Supported ? "yes" : "no");
            ImGui::Text("Reflex LowLatency Supported: %s", m_ui.REFLEX_LowLatencyAvailable ? "yes" : "no");
            if (m_ui.REFLEX_Supported && m_ui.REFLEX_LowLatencyAvailable) {
                ImGui::Combo("Reflex Low Latency", (int*)&m_ui.REFLEX_Mode, "Off\0On\0On + Boost\0");

                bool useFrameCap = m_ui.REFLEX_CapedFPS != 0;
                ImGui::Checkbox("Reflex FPS Capping", &useFrameCap);

                if (useFrameCap) {
                    if (m_ui.REFLEX_CapedFPS == 0) { m_ui.REFLEX_CapedFPS = 60; }
                    ImGui::SameLine();
                    ImGui::DragInt("##FPSReflexCap", &m_ui.REFLEX_CapedFPS, 1.f, 20, 240);
                }
                else {
                    m_ui.REFLEX_CapedFPS = 0;
                }

                if (ImGui::CollapsingHeader("Stats Report")) {
                    ImGui::Indent();
                    ImGui::Text(m_ui.REFLEX_Stats.c_str());
                    ImGui::Unindent();
                }
            }

            //
            // DLSS Frame Generatioon
            //

#ifdef DLSSG_ALLOWED // NDA ONLY DLSS-G DLSS_G Release
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Text, TITLE_COL);
            ImGui::Text("DLSS-G");
            ImGui::PopStyleColor();

            ImGui::Text("DLSS-G Supported: %s", m_ui.DLSSG_Supported ? "yes" : "no");
            if (m_ui.DLSSG_Supported) {

                if (m_ui.REFLEX_Mode == sl::ReflexMode::eOff) {
                    ImGui::Text("Reflex needs to be enabled for DLSSG to be enabled");
                    m_ui.DLSSG_mode = sl::DLSSGMode::eOff;
                }
                else {
                    ImGui::Combo("DLSS-G Mode", (int*)&m_ui.DLSSG_mode, "OFF\0ON");
                }


            }
#endif // DLSSG_ALLOWED END NDA ONLY DLSS-G DLSS_G Release

            //
            //  NIS Sharpenning
            //

            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Text, TITLE_COL);
            ImGui::Text("NIS Sharpenning");
            ImGui::PopStyleColor();

            ImGui::Text("NIS Supported: %s", m_ui.NIS_Supported ? "yes" : "no");

            if (m_ui.NIS_Supported){
                int nis_mode = m_ui.NIS_Mode == sl::NISMode::eScaler ? 1 : 0;
                ImGui::Combo("NIS Mode", &nis_mode, "Off\0On\0");
                m_ui.NIS_Mode = nis_mode == 1 ? sl::NISMode::eScaler : sl::NISMode::eOff;
                ImGui::DragFloat("Sharpness", &m_ui.NIS_Sharpness, 0.1f, 0, 2);
            }

            //
            //  Additional Settings
            //

            ImGui::Separator();

            if (ImGui::CollapsingHeader("Additional settings")) {

                ImGui::Indent();

                // Scene
                ImGui::PushStyleColor(ImGuiCol_Text, TITLE_COL);
                ImGui::Text("Scene");
                ImGui::PopStyleColor();

                // Scene 
                const std::string currentScene = m_app->GetCurrentSceneName();
                if (ImGui::BeginCombo("Scene", currentScene.c_str()))
                {
                    const std::vector<std::string>& scenes = m_app->GetAvailableScenes();
                    for (const std::string& scene : scenes)
                    {
                        bool is_selected = scene == currentScene;
                        if (ImGui::Selectable(scene.c_str(), is_selected))
                            m_app->SetCurrentSceneName(scene);
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                // Animation
                ImGui::Checkbox("Animate", &m_ui.EnableAnimations);
                if (m_ui.EnableAnimations) {
                    ImGui::SameLine();
                    ImGui::DragFloat("Speed", &m_ui.AnimationSpeed, 0.01f, 0.01f, 2.f);
                }

                ImGui::SliderFloat("Ambient Intensity", &m_ui.AmbientIntensity, 0.f, 1.f);

                ImGui::Checkbox("Enable Procedural Sky", &m_ui.EnableProceduralSky);
                if (m_ui.EnableProceduralSky && ImGui::CollapsingHeader("Sky Parameters"))
                {
                    ImGui::Indent();
                    ImGui::SliderFloat("Brightness", &m_ui.SkyParams.brightness, 0.f, 1.f);
                    ImGui::SliderFloat("Glow Size", &m_ui.SkyParams.glowSize, 0.f, 90.f);
                    ImGui::SliderFloat("Glow Sharpness", &m_ui.SkyParams.glowSharpness, 1.f, 10.f);
                    ImGui::SliderFloat("Glow Intensity", &m_ui.SkyParams.glowIntensity, 0.f, 1.f);
                    ImGui::SliderFloat("Horizon Size", &m_ui.SkyParams.horizonSize, 0.f, 90.f);
                    ImGui::Unindent();
                }

                // Additional Load
                ImGui::Separator();
                ImGui::PushStyleColor(ImGuiCol_Text, TITLE_COL);
                ImGui::Text("Additional Load");
                ImGui::PopStyleColor();

                // CPU Load
                bool enableCPULoad = m_ui.CpuLoad != 0;
                ImGui::Checkbox("Additional CPU Load", &enableCPULoad);
                if (enableCPULoad) {
                    if (m_ui.CpuLoad == 0) { m_ui.CpuLoad = 0.5; }
                    ImGui::SameLine();
                    ImGui::DragFloat("##CPULoad", &m_ui.CpuLoad, 1.f, 0.001f, 50);
                }
                else {
                    m_ui.CpuLoad = 0;
                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Runs a while loop for a given number of ms");


                // GPU Load
                bool enableGPULoad = m_ui.GpuLoad != 0;
                ImGui::Checkbox("Additional GPU Load", &enableGPULoad);
                if (enableGPULoad) {
                    if (m_ui.GpuLoad == 0) { m_ui.GpuLoad = 1; }
                    ImGui::SameLine();
                    ImGui::DragInt("##GPULoad", &m_ui.GpuLoad, 1.f, 1, 300);
                }
                else {
                    m_ui.GpuLoad = 0;
                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Repeats the Gbuffer pass an additional number of times");

                ImGui::Separator();
                ImGui::PushStyleColor(ImGuiCol_Text, TITLE_COL);
                ImGui::Text("Debug visualisation");
                ImGui::PopStyleColor();

                ImGui::Checkbox("Overlay Buffers", &m_ui.VisualiseBuffers);
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Shows the depth and motion vector buffers.");

                // Pipeline
                ImGui::Separator();
                ImGui::PushStyleColor(ImGuiCol_Text, TITLE_COL);
                ImGui::Text("Pipeline options");
                ImGui::PopStyleColor();

                ImGui::Checkbox("Enable SSAO", &m_ui.EnableSsao);
                ImGui::Checkbox("Enable Bloom", &m_ui.EnableBloom);

                if (m_ui.EnableBloom && ImGui::CollapsingHeader("Bloom Settings"))
                {
                    ImGui::Indent();
                    ImGui::DragFloat("Bloom Sigma", &m_ui.BloomSigma, 0.01f, 0.1f, 100.f);
                    ImGui::DragFloat("Bloom Alpha", &m_ui.BloomAlpha, 0.01f, 0.01f, 1.0f);
                    ImGui::Unindent();
                }

                ImGui::Checkbox("Enable Shadows", &m_ui.EnableShadows);
                ImGui::Checkbox("Enable Tonemapping", &m_ui.EnableToneMapping);
                if (m_ui.EnableToneMapping && ImGui::CollapsingHeader("ToneMapping Params"))
                {
                    ImGui::Indent();
                    ImGui::DragFloat("Exposure Bias", &m_ui.ToneMappingParams.exposureBias, 0.1f, -2, 2);
                    ImGui::Unindent();
                }


                ImGui::Unindent();

            }

        }

        ImGui::End();

        ImGui::PopFont();


        if (m_ui.MouseOverUI) {
            ImGui::SetNextWindowPos(ImVec2(width * 0.5f, height * 0.5f), 0, ImVec2(0.5f, 0.5f));
            ImGui::Begin("SettingText", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::PushFont(m_font_large);
            ImGui::Text("Settings Menu");
            ImGui::PopFont();
            ImGui::PushFont(m_font_medium);
            auto text = "sl::DLSSGMode::eOff is set.";
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize(text).x / 2);
            ImGui::Text(text);
            text = "Streamline features may behave differently while your mouse is hovering the UI.";
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize(text).x / 2);
            ImGui::Text(text);
            ImGui::PopFont();

            ImGui::End();
        }

    }
};
