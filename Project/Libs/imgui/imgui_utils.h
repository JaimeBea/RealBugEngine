#pragma once

#include "imgui.h"

/*
* This code is from
* https://gist.github.com/dougbinks/ef0962ef6ebe2cadae76c4e9f0586c69
* Author: https://gist.github.com/dougbinks
*/

#include <windows.h>
#include <shellapi.h>
#include <WinUser.h>
#include <cstdint>

#define ICON_FA_LINK "\xef\x83\x81"	// U+f0c1

namespace ImGui
{

	inline void ColumnSeparator(float cursor_pad = 4.f) {

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2 p = ImGui::GetCursorScreenPos();;
		draw_list->AddLine(ImVec2(p.x - 9999, p.y), ImVec2(p.x + 9999, p.y), ImGui::GetColorU32(ImGuiCol_Border));
		ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + cursor_pad));
	}

	inline void AddUnderLine(ImColor col_) {

		ImVec2 min = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();
		min.y = max.y;
		ImGui::GetWindowDrawList()->AddLine(min, max, col_, 1.0f);
	}

	// hyperlink urls
	inline void TextURL(const char* name_, const char* URL_, uint8_t SameLineBefore_ = 1, uint8_t SameLineAfter_ = 1) {

		if (1 == SameLineBefore_) { ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); }
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_UrlText]);
		ImGui::Text(name_);
		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered()) {

			if (ImGui::IsMouseClicked(0)) ShellExecute(NULL, "open", URL_, NULL, NULL, SW_SHOWNORMAL);
			AddUnderLine(ImGui::GetStyle().Colors[ImGuiCol_UrlText]);
			ImGui::SetTooltip(ICON_FA_LINK "  Open in browser\n%s", URL_);
		}
		else {
			AddUnderLine(ImGui::GetStyle().Colors[ImGuiCol_Button]);
		}
		if (1 == SameLineAfter_) { ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); }
	}
}