#include "PanelConsole.h"

#include "Logging.h"

#include "imgui.h"

static int ExecuteCommand(ImGuiInputTextCallbackData* data)
{
    return 0;
}

PanelConsole::PanelConsole() : Panel("Console", 600, 200) {}

void PanelConsole::Update()
{
    // Output
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::TextUnformattedWithColorMD(log_string.c_str());
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();

    ImGui::Separator();

    // Command-line
    bool reclaim_focus = false;
    char InputBuf[256] = { 0 };
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
    if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &ExecuteCommand))
    {
        log_string.append("$FFB86CFF # ");
        log_string.append(InputBuf);
        log_string.append("\n");
        reclaim_focus = true;
    }
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
}