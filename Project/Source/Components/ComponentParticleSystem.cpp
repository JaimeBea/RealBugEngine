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

void ComponentParticleSystem::OnEditorUpdate() {
	ImGui::TextColored(App->editor->textColor, "Texture Settings:");

	ImGui::Checkbox("Play: ", &isPlaying);
	ImGui::Checkbox("Loop: ", &looping);

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
	ImGui::Checkbox("Alpha transparency", &alphaTransparency);
	ImGui::Checkbox("Random Frame", &isRandomFrame);

	ImGui::ResourceSlot<ResourceShader>("shader", &shaderID);

	UID oldID = textureID;
	ImGui::ResourceSlot<ResourceTexture>("texture", &textureID);

	ResourceTexture* textureResource = App->resources->GetResource<ResourceTexture>(textureID);

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
		ImGui::InputInt("Xtiles: ", &Xtiles);
		ImGui::InputInt("Ytiles: ", &Ytiles);
		ImGui::InputFloat("Scale: ", &scale);
		ImGui::InputFloat("Life: ", &particleLife);

		if (ImGui::InputFloat("Speed: ", &velocity)) {
			CreateParticles(maxParticles, velocity);
		}

		if (ImGui::InputFloat("MaxParticles: ", &maxParticles)) {
			CreateParticles(maxParticles, velocity);
		}

		ImGui::ColorEdit4("InitColor##", initC.ptr());
		ImGui::ColorEdit4("FinalColor##", finalC.ptr());
	}
}

float3 ComponentParticleSystem::CreateVelocity() {
	if (emitterType == EmitterType::CONE) {
		ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
		float x = (float(rand()) / float((RAND_MAX)) * 0.2) - 0.2;
		float z = (float(rand()) / float((RAND_MAX)) * 0.2) - 0.2;
		float y = (float(rand()) / float((RAND_MAX)) * 1.0);
		return float3(x, y, z);
	}
	if (emitterType == EmitterType::SPHERE) {
		ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
		float x = (transform->GetGlobalPosition().x) + (float(rand()) / float((RAND_MAX)) * 2.0) - 1.0f;
		float z = (transform->GetGlobalPosition().z) + (float(rand()) / float((RAND_MAX)) * 2.0) - 1.0f;
		float y = (transform->GetGlobalPosition().y) + (float(rand()) / float((RAND_MAX)) * 2.0) - 1.0f;
		return float3(x, y, z);
	}
};

float3 ComponentParticleSystem::CreatePosition() {
	if (emitterType == EmitterType::CONE) {
		ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
		float x = (transform->GetGlobalPosition().x) + (float(rand()) / float((RAND_MAX)) * 0.2) - 0.2;
		float z = (transform->GetGlobalPosition().z) + (float(rand()) / float((RAND_MAX)) * 0.2) - 0.2;
		float y = (transform->GetGlobalPosition().y);
		return float3(x, y, z);
	}
	if (emitterType == EmitterType::SPHERE) {
		ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
		float x = (transform->GetGlobalPosition().x) + (float(rand()) / float((RAND_MAX)) * 0.5) - 0.5;
		float z = (transform->GetGlobalPosition().z) + (float(rand()) / float((RAND_MAX)) * 0.5) - 0.5;
		float y = (transform->GetGlobalPosition().y) + (float(rand()) / float((RAND_MAX)) * 0.5) - 0.5;
		return float3(transform->GetGlobalPosition());
	}
}

void ComponentParticleSystem::CreateParticles(float nParticles, float vel) {
	particles.Allocate(maxParticles);
	for (Particle& currentParticle : particles) {
		currentParticle.scale = float3(0.1f, 0.1f, 0.1f) * scale;
		currentParticle.initialPosition = CreatePosition();
		currentParticle.position = currentParticle.initialPosition;
		currentParticle.direction = CreateVelocity();
		currentParticle.velocity = vel;
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
	deadParticles.clear();
	for (Particle& currentParticle : particles) {
		currentParticle.life -= 0.01;
		currentParticle.position += currentParticle.direction * velocity;
		currentParticle.model = float4x4::FromTRS(currentParticle.position, currentParticle.rotation, currentParticle.scale);
		if (currentParticle.life < 0) {
			deadParticles.push_back(&currentParticle);
		}
	}
	for (Particle* currentParticle : deadParticles) {
		particles.Release(currentParticle);
	}
	Draw();
}

void ComponentParticleSystem::Init() {
	CreateParticles(maxParticles, velocity);
}

void ComponentParticleSystem::SpawnParticle() {
	Particle* currentParticle = particles.Obtain();
	if (currentParticle) {
		currentParticle->position = currentParticle->initialPosition;
		currentParticle->life = particleLife;
		if (isRandomFrame) {
			currentParticle->currentFrame = rand() % ((Xtiles * Ytiles) + 1);
		} else {
			currentParticle->currentFrame = 0;
		}
		currentParticle->colorFrame = 0;
		currentParticle->direction = CreateVelocity();
		currentParticle->initialPosition = CreatePosition();
		currentParticle->position = currentParticle->initialPosition;
		currentParticle->scale = float3(0.1f, 0.1f, 0.1f) * scale;
	}
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
	if (isPlaying) {
		for (Particle& currentParticle : particles) {
			unsigned int program = 0;
			ResourceShader* shaderResouce = App->resources->GetResource<ResourceShader>(shaderID);
			if (shaderResouce) {
				program = shaderResouce->GetShaderProgram();
			} else {
				return;
			}

			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			//glBlendEquation(GL_MAX);
			glBlendFunc(GL_ONE, GL_ONE);

			glBindBuffer(GL_ARRAY_BUFFER, App->userInterface->GetQuadVBO());
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(float) * 6 * 3));
			glUseProgram(program);
			//TODO ADD DELTATIME

			ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
			currentParticle.model = currentParticle.model;

			float3x3 rotatePart = currentParticle.model.RotatePart();
			Frustum* frustum = App->camera->GetActiveCamera()->GetFrustum();
			float4x4* proj = &App->camera->GetProjectionMatrix();
			float4x4* view = &App->camera->GetViewMatrix();

			float4x4 newModelMatrix = currentParticle.model.LookAt(rotatePart.Col(2), -frustum->Front(), rotatePart.Col(1), float3::unitY);
			float4x4 Final = float4x4::FromTRS(currentParticle.position, newModelMatrix.RotatePart(), currentParticle.scale);
			//-> glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, newModelMatrix.ptr());

			glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, Final.ptr());
			glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view->ptr());
			glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, proj->ptr());

			if (!isRandomFrame) {
				currentParticle.currentFrame += 0.1f;
			}

			currentParticle.colorFrame += 0.01f;
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(glGetUniformLocation(program, "diffuse"), 0);
			glUniform1f(glGetUniformLocation(program, "currentFrame"), currentParticle.currentFrame);
			glUniform1f(glGetUniformLocation(program, "colorFrame"), currentParticle.colorFrame);
			glUniform4fv(glGetUniformLocation(program, "initColor"), 1, initC.ptr());
			glUniform4fv(glGetUniformLocation(program, "finalColor"), 1, finalC.ptr());

			glUniform1i(glGetUniformLocation(program, "Xtiles"), Xtiles);
			glUniform1i(glGetUniformLocation(program, "Ytiles"), Ytiles);

			ResourceTexture* textureResource = App->resources->GetResource<ResourceTexture>(textureID);
			if (textureResource != nullptr) {
				glBindTexture(GL_TEXTURE_2D, textureResource->glTexture);
			}
			//glDrawArraysInstanced(GL_TRIANGLES, 0, 4, 100);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDisable(GL_BLEND);
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
		}
		SpawnParticle();
	}
}