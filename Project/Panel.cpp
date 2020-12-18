#include "Panel.h"

#include "Leaks.h"

Panel::Panel(const char* name, bool enabled) : name(name), enabled(enabled) {}

void Panel::Update() {}

void Panel::HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
