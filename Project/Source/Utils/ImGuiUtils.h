#pragma once

#include "Application.h"
#include "Modules/ModuleResources.h"

#include "imgui.h"

namespace ImGui {
	template<typename T>
	void ResourceSlot(const char* label, UID* target) {
		ImGui::Text(label);
		ImGui::BeginChildFrame(ImGui::GetID(target), ImVec2(32, 32));
		ImGui::EndChildFrame();

		if (ImGui::BeginDragDropTarget()) {
			std::string payloadType = std::string("_RESOURCE_") + GetResourceTypeName(T::staticType);
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadType.c_str())) {
				if (*target != 0) {
					T* oldResource = (T*) App->resources->GetResourceByID(*target);
					oldResource->DecreaseReferenceCount();
				}
				*target = *(UID*) payload->Data;
				T* payloadResource = (T*) App->resources->GetResourceByID(*target);
				payloadResource->IncreaseReferenceCount();
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();
		std::string text = std::string("Id: ") + std::to_string(*target);
		ImGui::Text(text.c_str());
	}
}