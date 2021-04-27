#include "ComponentParticleSystem.h"

#include "GameObject.h"
#include "Components/ComponentTransform.h"
#include "Components/UI/ComponentButton.h"
#include "Application.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleUserInterface.h"
#include "Panels/PanelScene.h"
#include "Resources/ResourceTexture.h"
#include "Resources/ResourceShader.h"
#include "FileSystem/TextureImporter.h"
#include "FileSystem/JsonValue.h"
#include "Math/float3x3.h"
#include "Utils/ImGuiUtils.h"
#include "Math/TransformOps.h"
#include "imgui.h"
#include "GL/glew.h"
#include "debugdraw.h"

#include "Utils/Leaks.h"

#define JSON_TAG_TEXTURE_SHADERID "ShaderId"
#define JSON_TAG_TEXTURE_TEXTUREID "TextureId"
#define JSON_TAG_COLOR "Color"
#define JSON_TAG_ALPHATRANSPARENCY "AlphaTransparency"

#include <random>

// clang-format off
float quadVertices[] = {
    // positions     // colors
    -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
    -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

    -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
     0.05f,  0.05f,  0.0f, 1.0f, 1.0f
};
// clang-format on

void ComponentParticleSystem::OnEditorUpdate() {
	ImGui::TextColored(App->editor->textColor, "Texture Settings:");

	ImGui::ColorEdit4("Color##", color.ptr());

	ImGui::Checkbox("Alpha transparency", &alphaTransparency);

	ImGui::ResourceSlot<ResourceShader>("shader", &shaderID);

	UID oldID = textureID;
	ImGui::ResourceSlot<ResourceTexture>("texture", &textureID);

	ResourceTexture* textureResource = (ResourceTexture*) App->resources->GetResource(textureID);

	if (textureResource != nullptr) {
		int width;
		int height;
		glGetTextureLevelParameteriv(textureResource->glTexture, 0, GL_TEXTURE_WIDTH, &width);
		glGetTextureLevelParameteriv(textureResource->glTexture, 0, GL_TEXTURE_HEIGHT, &height);

		if (oldID != textureID) {
			ComponentTransform2D* transform2D = GetOwner().GetComponent<ComponentTransform2D>();
			if (transform2D != nullptr) {
				transform2D->SetSize(float2(width, height));
			}
		}

		ImGui::Text("");
		ImGui::Separator();
		ImGui::TextColored(App->editor->titleColor, "Texture Preview");
		ImGui::TextWrapped("Size:");
		ImGui::SameLine();
		ImGui::TextWrapped("%d x %d", width, height);
		ImGui::Image((void*) textureResource->glTexture, ImVec2(200, 200));
		ImGui::Separator();
	}

	if (ImGui::InputFloat("Speed: ", &velocity)) {
		CreateParticles(maxParticles, velocity);
	}

	if (ImGui::InputFloat("MaxParticles: ", &maxParticles)) {
		CreateParticles(maxParticles, velocity);
	}

	ImGui::Separator();
	const char* emitterTypeCombo[] = {"Cone", "Sphere", "Hemisphere", "Donut", "Circle", "Rectangle"};
	const char* emitterTypeComboCurrent = emitterTypeCombo[(int) emitterType];
	ImGui::TextColored(App->editor->textColor, "Shape:");
	if (ImGui::BeginCombo("##Shape", emitterTypeComboCurrent)) {
		for (int n = 0; n < IM_ARRAYSIZE(emitterTypeCombo); ++n) {
			bool isSelected = (emitterTypeComboCurrent == emitterTypeCombo[n]);
			if (ImGui::Selectable(emitterTypeCombo[n], isSelected)) {
				emitterType = (EmitterType) n;
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::ColorEdit4("InitColor##", initC.ptr());
	ImGui::ColorEdit4("FinalColor##", finalC.ptr());
}

float3 ComponentParticleSystem::CreatePosition() {
	if (emitterType == EmitterType::CONE) {
		ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
		float x = (float(rand()) / float((RAND_MAX)) * 1.0) - 1.0f;
		float z = (float(rand()) / float((RAND_MAX)) * 1.0) - 1.0f;
		float y = (transform->GetPosition().y);
		return float3(x, y, z);
	}
}

void ComponentParticleSystem::CreateParticles(float nParticles, float vel) {
	particles.clear();
	for (int i = 0; i < nParticles; i++) {
		particles.push_back(Particle(shaderID, textureID, true, vel, initC, finalC, CreatePosition()));
	}
}
void ComponentParticleSystem::Load(JsonValue jComponent) {
	shaderID = jComponent[JSON_TAG_TEXTURE_SHADERID];

	if (shaderID != 0) {
		App->resources->IncreaseReferenceCount(shaderID);
	}

	textureID = jComponent[JSON_TAG_TEXTURE_TEXTUREID];

	if (textureID != 0) {
		App->resources->IncreaseReferenceCount(textureID);
	}

	alphaTransparency = jComponent[JSON_TAG_ALPHATRANSPARENCY];
	CreateParticles(maxParticles, velocity);
}

void ComponentParticleSystem::Save(JsonValue jComponent) const {
}

void ComponentParticleSystem::Update() {
	aux = 0;
	bool ret = true;
	for (std::vector<Particle>::iterator it = particles.begin(); it != particles.end() && ret; ++it) {
		if ((it)->isActivate) {
			(it)->Uptade();
			(it)->Draw(GetOwner().GetComponent<ComponentTransform>());
			if (aux < 100) {
				translations[aux].Add((it)->position);
				aux++;
			}
		}
	}
	Draw();
}

void ComponentParticleSystem::DrawGizmos() {
	if (IsActiveInHierarchy()) {
		if (emitterType == EmitterType::CONE) {
			ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
			dd::cone(transform->GetGlobalPosition(), transform->GetGlobalRotation() * float3::unitY * 1, dd::colors::White, 1.0f, 0.3f);
		}
		if (emitterType == EmitterType::SPHERE) {
			float delta = kl * kl - 4 * (kc - 10) * kq;
			float distance = Max(abs((-kl + sqrt(delta))) / (2 * kq), abs((-kl - sqrt(delta)) / (2 * kq)));
			ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
			dd::sphere(transform->GetGlobalPosition(), dd::colors::White, 1.0f);
		}
	}
}

void ComponentParticleSystem::DuplicateComponent(GameObject& owner) {
}

const float4& ComponentParticleSystem::GetTintColor() const {
	ComponentButton* button = GetOwner().GetComponent<ComponentButton>();
	if (button != nullptr) {
		return button->GetTintColor();
	}
	return float4::one;
}

void ComponentParticleSystem::Draw() {
	bool ret = true;
	for (std::vector<Particle>::iterator it = particles.begin(); it != particles.end() && ret; ++it) {
		if (!(it)->isActivate) {
			(it)->Spawn(CreatePosition());
			(it)->Draw(GetOwner().GetComponent<ComponentTransform>());
			return;
		}
	}
}