#include "ComponentMaterial.h"

#include "Application.h"
#include "FileSystem/TextureImporter.h"
#include "Resources/GameObject.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleScene.h"

#include "imgui.h"
#include "GL/glew.h"

#include "Utils/Leaks.h"

#define JSON_TAG_HAS_DIFFUSE_MAP "HasDiffuseMap"
#define JSON_TAG_DIFFUSE_COLOR "DiffuseColor"
#define JSON_TAG_DIFFUSE_MAP_FILE_NAME "DiffuseMapFileName"
#define JSON_TAG_HAS_SPECULAR_MAP "HasSpecularMap"
#define JSON_TAG_SPECULAR_COLOR "SpecularColor"
#define JSON_TAG_HAS_SPECULAR_MAP_FILE_NAME "SpecularMapFileName"
#define JSON_TAG_SHININESS "Shininess"
#define JSON_TAG_HAS_SHININESS_IN_ALPHA_CHANNEL "HasShininessInAlphaChannel"
#define JSON_TAG_AMBIENT "Ambient"

void ComponentMaterial::OnEditorUpdate() {
	if (ImGui::CollapsingHeader("Material")) {
		bool active = IsActive();
		if (ImGui::Checkbox("Active", &active)) {
			active ? Enable() : Disable();
		}
		ImGui::SameLine();

		if (ImGui::Button("Remove")) {
			// TODO: Fix me
			// selected->RemoveComponent(material);
			// continue;
		}
		ImGui::Separator();

		ImGui::TextColored(App->editor->titleColor, "Shader");

		// Material types
		const char* materialTypes[] = {"Standard", "Phong"};
		const char* materialTypesCurrent = materialTypes[(int) material.materialType];
		if (ImGui::BeginCombo("Type", materialTypesCurrent)) {
			for (int n = 0; n < IM_ARRAYSIZE(materialTypes); ++n) {
				bool isSelected = (materialTypesCurrent == materialTypes[n]);
				if (ImGui::Selectable(materialTypes[n], isSelected)) {
					material.materialType = ShaderType(n);
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
			ImGui::Text("");
		}
		if (material.materialType == ShaderType::PHONG) {
			std::vector<Texture*> textures;
			for (Texture& texture : App->resources->textures) textures.push_back(&texture);

			// Diffuse Texture Combo
			const char* diffuseItems[] = {"Diffuse Color", "Diffuse Texture"};
			const char* diffuseItemCurrent = diffuseItems[material.hasDiffuseMap];
			ImGui::TextColored(App->editor->textColor, "Diffuse Settings:");
			if (ImGui::BeginCombo("##diffuse", diffuseItemCurrent)) {
				for (int n = 0; n < IM_ARRAYSIZE(diffuseItems); ++n) {
					bool isSelected = (diffuseItemCurrent == diffuseItems[n]);
					if (ImGui::Selectable(diffuseItems[n], isSelected)) {
						material.hasDiffuseMap = n ? 1 : 0;
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			if (diffuseItemCurrent == diffuseItems[0]) {
				ImGui::ColorEdit3("Color##diffuse", material.diffuseColor.ptr());
			} else {
				std::string& currentDiffuseTexture = material.diffuseMap ? material.diffuseMap->fileName : "";
				if (ImGui::BeginCombo("Texture##diffuse", currentDiffuseTexture.c_str())) {
					for (unsigned i = 0; i < textures.size(); ++i) {
						bool isSelected = (currentDiffuseTexture == textures[i]->fileName);
						if (ImGui::Selectable(textures[i]->fileName.c_str(), isSelected)) {
							material.diffuseMap = textures[i];
						};
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
			ImGui::Text("");

			// Specular Texture Combo
			const char* specularItems[] = {"Specular Color", "Specular Texture"};
			const char* specularItemCurrent = specularItems[material.hasSpecularMap];
			ImGui::TextColored(App->editor->textColor, "Specular Settings:");
			if (ImGui::BeginCombo("##specular", specularItemCurrent)) {
				for (int n = 0; n < IM_ARRAYSIZE(specularItems); ++n) {
					bool isSelected = (specularItemCurrent == specularItems[n]);
					if (ImGui::Selectable(specularItems[n], isSelected)) {
						material.hasSpecularMap = n ? 1 : 0;
					};
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			if (specularItemCurrent == specularItems[0]) {
				ImGui::ColorEdit3("Color##specular", material.specularColor.ptr());
			} else {
				std::string& currentSpecularTexture = material.specularMap ? material.specularMap->fileName : "";
				if (ImGui::BeginCombo("Texture##specular", currentSpecularTexture.c_str())) {
					for (unsigned i = 0; i < textures.size(); ++i) {
						bool isSelected = (currentSpecularTexture == textures[i]->fileName);
						if (ImGui::Selectable(textures[i]->fileName.c_str(), isSelected)) {
							material.specularMap = textures[i];
						};
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}

			// Shininess Combo
			const char* shininessItems[] = {"Shininess Value", "Shininess Alpha"};
			const char* shininessItemCurrent = shininessItems[material.hasShininessInAlphaChannel];
			if (ImGui::BeginCombo("##shininess", shininessItemCurrent)) {
				for (int n = 0; n < IM_ARRAYSIZE(shininessItems); ++n) {
					bool isSelected = (shininessItemCurrent == shininessItems[n]);
					if (ImGui::Selectable(shininessItems[n], isSelected)) {
						material.hasShininessInAlphaChannel = n ? 1 : 0;
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			if (shininessItemCurrent == shininessItems[0]) {
				ImGui::DragFloat("Shininess##shininess", &material.shininess, App->editor->dragSpeed3f, 0.0f, 1000.0f);
			}
		}
		ImGui::Separator();
		ImGui::TextColored(App->editor->titleColor, "Filters");

		// Min filter combo box
		const char* minFilterItems[] = {"Nearest", "Linear", "Nearest Mipmap Nearest", "Linear Mipmap Nearest", "Nearest Mipmap Linear", "Linear Mipmap Linear"};
		const char* minFilterItemCurrent = minFilterItems[(int) App->resources->GetMinFilter()];
		if (ImGui::BeginCombo("Min filter", minFilterItemCurrent)) {
			for (int n = 0; n < IM_ARRAYSIZE(minFilterItems); ++n) {
				bool isSelected = (minFilterItemCurrent == minFilterItems[n]);
				if (ImGui::Selectable(minFilterItems[n], isSelected)) {
					App->resources->SetMinFilter(TextureMinFilter(n));
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		// Mag filter combo box
		const char* magFilterItems[] = {"Nearest", "Linear"};
		const char* magFilterItemCurrent = magFilterItems[(int) App->resources->GetMagFilter()];
		if (ImGui::BeginCombo("Mag filter", magFilterItemCurrent)) {
			for (int n = 0; n < IM_ARRAYSIZE(magFilterItems); ++n) {
				bool isSelected = (magFilterItemCurrent == magFilterItems[n]);
				if (ImGui::Selectable(magFilterItems[n], isSelected)) {
					App->resources->SetMagFilter(TextureMagFilter(n));
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		// Texture wrap combo box
		const char* wrapItems[] = {"Repeat", "Clamp to Edge", "Clamp to Border", "Mirrored Repeat", "Mirrored Clamp to Edge"};
		const char* wrapItemCurrent = wrapItems[(int) App->resources->GetWrap()];
		if (ImGui::BeginCombo("Wrap", wrapItemCurrent)) {
			for (int n = 0; n < IM_ARRAYSIZE(wrapItems); ++n) {
				bool isSelected = (wrapItemCurrent == wrapItems[n]);
				if (ImGui::Selectable(wrapItems[n], isSelected)) {
					App->resources->SetWrap(TextureWrap(n));
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Separator();
		if (material.diffuseMap != nullptr) {
			ImGui::TextColored(App->editor->titleColor, "Diffuse Texture");
			ImGui::TextWrapped("Size:##diffuse");
			ImGui::SameLine();
			int width;
			int height;
			glGetTextureLevelParameteriv(material.diffuseMap->glTexture, 0, GL_TEXTURE_WIDTH, &width);
			glGetTextureLevelParameteriv(material.diffuseMap->glTexture, 0, GL_TEXTURE_HEIGHT, &height);
			ImGui::TextWrapped("%d x %d##diffuse", width, height);
			ImGui::Image((void*) material.diffuseMap->glTexture, ImVec2(200, 200));
			ImGui::Separator();
		}
		if (material.specularMap != nullptr) {
			ImGui::TextColored(App->editor->titleColor, "Specular Texture");
			ImGui::TextWrapped("Size:##specular");
			ImGui::SameLine();
			int width;
			int height;
			glGetTextureLevelParameteriv(material.specularMap->glTexture, 0, GL_TEXTURE_WIDTH, &width);
			glGetTextureLevelParameteriv(material.specularMap->glTexture, 0, GL_TEXTURE_HEIGHT, &height);
			ImGui::TextWrapped("%d x %d##specular", width, height);
			ImGui::Image((void*) material.specularMap->glTexture, ImVec2(200, 200));
			ImGui::Separator();
		}
	}
}

void ComponentMaterial::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_HAS_DIFFUSE_MAP] = material.hasDiffuseMap;
	JsonValue jDiffuseColor = jComponent[JSON_TAG_DIFFUSE_COLOR];
	jDiffuseColor[0] = material.diffuseColor.x;
	jDiffuseColor[1] = material.diffuseColor.y;
	jDiffuseColor[2] = material.diffuseColor.z;
	if (material.hasDiffuseMap) jComponent[JSON_TAG_DIFFUSE_MAP_FILE_NAME] = material.diffuseMap->fileName.c_str();

	jComponent[JSON_TAG_HAS_SPECULAR_MAP] = material.hasSpecularMap;
	JsonValue jSpecularColor = jComponent[JSON_TAG_SPECULAR_COLOR];
	jSpecularColor[0] = material.specularColor.x;
	jSpecularColor[1] = material.specularColor.y;
	jSpecularColor[2] = material.specularColor.z;
	if (material.hasSpecularMap) jComponent[JSON_TAG_HAS_SPECULAR_MAP_FILE_NAME] = material.specularMap->fileName.c_str();

	jComponent[JSON_TAG_SHININESS] = material.shininess;
	jComponent[JSON_TAG_HAS_SHININESS_IN_ALPHA_CHANNEL] = material.hasShininessInAlphaChannel;

	JsonValue jAmbient = jComponent[JSON_TAG_AMBIENT];
	jAmbient[0] = material.ambient.x;
	jAmbient[1] = material.ambient.y;
	jAmbient[2] = material.ambient.z;
}

void ComponentMaterial::Load(JsonValue jComponent) {
	material.hasDiffuseMap = jComponent[JSON_TAG_HAS_DIFFUSE_MAP];
	JsonValue jDiffuseColor = jComponent[JSON_TAG_DIFFUSE_COLOR];
	material.diffuseColor.Set(jDiffuseColor[0], jDiffuseColor[1], jDiffuseColor[2]);
	if (material.hasDiffuseMap) {
		std::string diffuseFileName = jComponent[JSON_TAG_DIFFUSE_MAP_FILE_NAME];
		for (Texture& texture : App->resources->textures) {
			if (texture.fileName == diffuseFileName) {
				material.diffuseMap = &texture;
			}
		}
		if (material.diffuseMap == nullptr) {
			material.diffuseMap = App->resources->ObtainTexture();
			material.diffuseMap->fileName = diffuseFileName;
		}

		TextureImporter::UnloadTexture(material.diffuseMap);
		TextureImporter::LoadTexture(material.diffuseMap);
	} else if (material.diffuseMap != nullptr) {
		App->resources->ReleaseTexture(material.diffuseMap);
		material.diffuseMap = nullptr;
	}

	material.hasSpecularMap = jComponent[JSON_TAG_HAS_SPECULAR_MAP];
	JsonValue jSpecularColor = jComponent[JSON_TAG_SPECULAR_COLOR];
	material.specularColor.Set(jSpecularColor[0], jSpecularColor[1], jSpecularColor[2]);
	if (material.hasSpecularMap) {
		std::string specularFileName = jComponent[JSON_TAG_HAS_SPECULAR_MAP_FILE_NAME];
		for (Texture& texture : App->resources->textures) {
			if (texture.fileName == specularFileName) {
				material.specularMap = &texture;
			}
		}
		if (material.specularMap == nullptr) {
			material.specularMap = App->resources->ObtainTexture();
			material.specularMap->fileName = specularFileName;
		}

		TextureImporter::UnloadTexture(material.specularMap);
		TextureImporter::LoadTexture(material.specularMap);
	} else if (material.specularMap != nullptr) {
		App->resources->ReleaseTexture(material.specularMap);
		material.specularMap = nullptr;
	}

	material.shininess = jComponent[JSON_TAG_SHININESS];
	material.hasShininessInAlphaChannel = jComponent[JSON_TAG_HAS_SHININESS_IN_ALPHA_CHANNEL];

	JsonValue jAmbient = jComponent[JSON_TAG_AMBIENT];
	material.ambient.Set(jAmbient[0], jAmbient[1], jAmbient[2]);
}
