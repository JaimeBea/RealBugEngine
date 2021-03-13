#include "ComponentMeshRenderer.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "FileSystem/MeshImporter.h"
#include "FileSystem/TextureImporter.h"
#include "Resources/GameObject.h"
#include "Resources/Texture.h"
#include "Resources/Mesh.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentLight.h"
#include "Components/ComponentBoundingBox.h"
#include "Components/ComponentAnimation.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleTime.h"

#include "assimp/mesh.h"
#include "GL/glew.h"
#include "imgui.h"

#include "Utils/Leaks.h"

#define JSON_TAG_FILENAME "FileName"
#define JSON_TAG_MATERIAL_INDEX "MaterialIndex"

// TODO: Move to Material Importer class

#define JSON_TAG_MATERIAL "Material"
#define JSON_TAG_HAS_DIFFUSE_MAP "HasDiffuseMap"
#define JSON_TAG_DIFFUSE_COLOR "DiffuseColor"
#define JSON_TAG_DIFFUSE_MAP_FILE_NAME "DiffuseMapFileName"
#define JSON_TAG_HAS_SPECULAR_MAP "HasSpecularMap"
#define JSON_TAG_SPECULAR_COLOR "SpecularColor"
#define JSON_TAG_HAS_SPECULAR_MAP_FILE_NAME "SpecularMapFileName"
#define JSON_TAG_SHININESS "Shininess"
#define JSON_TAG_HAS_SHININESS_IN_ALPHA_CHANNEL "HasShininessInAlphaChannel"
#define JSON_TAG_AMBIENT "Ambient"

void ComponentMeshRenderer::OnEditorUpdate() {
	bool active = IsActive();
	if (ImGui::Checkbox("Active", &active)) {
		active ? Enable() : Disable();
	}
	ImGui::Separator();
	// MESH
	if (ImGui::TreeNode("Mesh")) {
		ImGui::TextColored(App->editor->titleColor, "Geometry");
		ImGui::TextWrapped("Num Vertices: ");
		ImGui::SameLine();
		ImGui::TextColored(App->editor->textColor, "%d", mesh->numVertices);
		ImGui::TextWrapped("Num Triangles: ");
		ImGui::SameLine();
		ImGui::TextColored(App->editor->textColor, "%d", mesh->numIndices / 3);
		ImGui::TreePop();
	}
	// MATERIAL
	if (ImGui::TreeNode("Material")) {
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
		if (ImGui::BeginTabBar("TexturesTab")) {
			if (material.diffuseMap != nullptr) {
				if (ImGui::BeginTabItem("Diffuse Texture", nullptr, ImGuiTabItemFlags_None)) {
					int width;
					int height;
					glGetTextureLevelParameteriv(material.diffuseMap->glTexture, 0, GL_TEXTURE_WIDTH, &width);
					glGetTextureLevelParameteriv(material.diffuseMap->glTexture, 0, GL_TEXTURE_HEIGHT, &height);
					ImGui::TextWrapped("Size: %d x %d", width, height);
					ImGui::Image((void*) material.diffuseMap->glTexture, ImVec2(200, 200));
					ImGui::EndTabItem();
				}
			}
			if (material.specularMap != nullptr) {
				if (ImGui::BeginTabItem("Specular Texture", nullptr, ImGuiTabItemFlags_None)) {
					int width;
					int height;
					glGetTextureLevelParameteriv(material.specularMap->glTexture, 0, GL_TEXTURE_WIDTH, &width);
					glGetTextureLevelParameteriv(material.specularMap->glTexture, 0, GL_TEXTURE_HEIGHT, &height);
					ImGui::TextWrapped("Size: %d x %d", width, height);
					ImGui::Image((void*) material.specularMap->glTexture, ImVec2(200, 200));
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
		ImGui::TreePop();
	}
}

void ComponentMeshRenderer::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_FILENAME] = mesh->fileName.c_str();
	jComponent[JSON_TAG_MATERIAL_INDEX] = mesh->materialIndex;

	// TODO: Save only the Material FileName when saved in separate resource

	JsonValue jMaterial = jComponent[JSON_TAG_MATERIAL];

	jMaterial[JSON_TAG_HAS_DIFFUSE_MAP] = material.hasDiffuseMap;
	JsonValue jDiffuseColor = jMaterial[JSON_TAG_DIFFUSE_COLOR];
	jDiffuseColor[0] = material.diffuseColor.x;
	jDiffuseColor[1] = material.diffuseColor.y;
	jDiffuseColor[2] = material.diffuseColor.z;
	if (material.hasDiffuseMap) jMaterial[JSON_TAG_DIFFUSE_MAP_FILE_NAME] = material.diffuseMap->fileName.c_str();

	jMaterial[JSON_TAG_HAS_SPECULAR_MAP] = material.hasSpecularMap;
	JsonValue jSpecularColor = jMaterial[JSON_TAG_SPECULAR_COLOR];
	jSpecularColor[0] = material.specularColor.x;
	jSpecularColor[1] = material.specularColor.y;
	jSpecularColor[2] = material.specularColor.z;
	if (material.hasSpecularMap) jMaterial[JSON_TAG_HAS_SPECULAR_MAP_FILE_NAME] = material.specularMap->fileName.c_str();

	jMaterial[JSON_TAG_SHININESS] = material.shininess;
	jMaterial[JSON_TAG_HAS_SHININESS_IN_ALPHA_CHANNEL] = material.hasShininessInAlphaChannel;

	JsonValue jAmbient = jMaterial[JSON_TAG_AMBIENT];
	jAmbient[0] = material.ambient.x;
	jAmbient[1] = material.ambient.y;
	jAmbient[2] = material.ambient.z;
}

void ComponentMeshRenderer::Load(JsonValue jComponent) {
	std::string fileName = jComponent[JSON_TAG_FILENAME];
	for (Mesh& otherMesh : App->resources->meshes) {
		if (otherMesh.fileName == fileName) {
			mesh = &otherMesh;
		}
	}
	if (mesh == nullptr) {
		mesh = App->resources->ObtainMesh();
		mesh->fileName = fileName;
	}
	mesh->materialIndex = jComponent[JSON_TAG_MATERIAL_INDEX];

	MeshImporter::UnloadMesh(mesh);
	MeshImporter::LoadMesh(mesh);

	// TODO: Load using Material Importer

	JsonValue jMaterial = jComponent[JSON_TAG_MATERIAL];

	material.hasDiffuseMap = jMaterial[JSON_TAG_HAS_DIFFUSE_MAP];
	JsonValue jDiffuseColor = jMaterial[JSON_TAG_DIFFUSE_COLOR];
	material.diffuseColor.Set(jDiffuseColor[0], jDiffuseColor[1], jDiffuseColor[2]);
	if (material.hasDiffuseMap) {
		std::string diffuseFileName = jMaterial[JSON_TAG_DIFFUSE_MAP_FILE_NAME];
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

	material.hasSpecularMap = jMaterial[JSON_TAG_HAS_SPECULAR_MAP];
	JsonValue jSpecularColor = jMaterial[JSON_TAG_SPECULAR_COLOR];
	material.specularColor.Set(jSpecularColor[0], jSpecularColor[1], jSpecularColor[2]);
	if (material.hasSpecularMap) {
		std::string specularFileName = jMaterial[JSON_TAG_HAS_SPECULAR_MAP_FILE_NAME];
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

	material.shininess = jMaterial[JSON_TAG_SHININESS];
	material.hasShininessInAlphaChannel = jMaterial[JSON_TAG_HAS_SHININESS_IN_ALPHA_CHANNEL];

	JsonValue jAmbient = jMaterial[JSON_TAG_AMBIENT];
	material.ambient.Set(jAmbient[0], jAmbient[1], jAmbient[2]);
}

void ComponentMeshRenderer::Draw(const float4x4& modelMatrix) {
	if (!IsActive()) return;

	if (mesh->numBones > 0 && App->time->IsGameRunning()) {
		SkinningCPU();
	}

	unsigned program = App->programs->defaultProgram;

	float4x4 viewMatrix = App->camera->GetViewMatrix();
	float4x4 projMatrix = App->camera->GetProjectionMatrix();
	unsigned glTextureDiffuse = 0;
	unsigned glTextureSpecular = 0;

	Texture* diffuse = material.diffuseMap;
	glTextureDiffuse = diffuse ? diffuse->glTexture : 0;
	Texture* specular = material.specularMap;
	glTextureSpecular = specular ? specular->glTexture : 0;

	ComponentLight* directionalLight = nullptr;
	std::vector<ComponentLight*> pointLightsVector;
	std::vector<float> pointDistancesVector;
	std::vector<ComponentLight*> spotLightsVector;
	std::vector<float> spotDistancesVector;

	if (material.materialType == ShaderType::PHONG) {
		float farPointDistance = 0;
		ComponentLight* farPointLight = nullptr;
		float farSpotDistance = 0;
		ComponentLight* farSpotLight = nullptr;

		for (GameObject& object : App->scene->gameObjects) {
			ComponentLight* light = object.GetComponent<ComponentLight>();
			if (light == nullptr) continue;

			if (light->lightType == LightType::DIRECTIONAL) {
				// It takes the first actived Directional Light inside the Pool
				if (light->IsActive() && directionalLight == nullptr) {
					directionalLight = light;
					continue;
				}
			} else if (light->lightType == LightType::POINT) {
				if (light->IsActive()) {
					float3 meshPosition = GetOwner().GetComponent<ComponentTransform>()->GetPosition();
					float3 lightPosition = object.GetComponent<ComponentTransform>()->GetPosition();
					float distance = Distance(meshPosition, lightPosition);
					if (pointLightsVector.size() < 8) {
						pointDistancesVector.push_back(distance);
						pointLightsVector.push_back(light);

						if (distance > farPointDistance) {
							farPointLight = light;
							farPointDistance = distance;
						}
					} else {
						if (distance < farPointDistance) {
							int count = 0;
							int selected = -1;
							for (float pointDistance : pointDistancesVector) {
								if (pointDistance == farPointDistance) selected = count;
								count += 1;
							}

							pointLightsVector[selected] = light;
							pointDistancesVector[selected] = distance;

							count = 0;
							selected = -1;
							float maxDistance = 0;
							for (float pointDistance : pointDistancesVector) {
								if (pointDistance > maxDistance) {
									maxDistance = pointDistance;
									selected = count;
								}
								count += 1;
							}

							farPointDistance = maxDistance;
							farPointLight = pointLightsVector[selected];
						}
					}
				}
			} else if (light->lightType == LightType::SPOT) {
				if (light->IsActive()) {
					float3 meshPosition = GetOwner().GetComponent<ComponentTransform>()->GetPosition();
					float3 lightPosition = object.GetComponent<ComponentTransform>()->GetPosition();
					float distance = Distance(meshPosition, lightPosition);
					if (spotLightsVector.size() < 8) {
						spotDistancesVector.push_back(distance);
						spotLightsVector.push_back(light);

						if (distance > farSpotDistance) {
							farSpotLight = light;
							farSpotDistance = distance;
						}
					} else {
						if (distance < farSpotDistance) {
							int count = 0;
							int selected = -1;
							for (float spotDistance : spotDistancesVector) {
								if (spotDistance == farSpotDistance) selected = count;
								count += 1;
							}

							spotLightsVector[selected] = light;
							spotDistancesVector[selected] = distance;

							count = 0;
							selected = -1;
							float maxDistance = 0;
							for (float spotDistance : spotDistancesVector) {
								if (spotDistance > maxDistance) {
									maxDistance = spotDistance;
									selected = count;
								}
								count += 1;
							}

							farSpotDistance = maxDistance;
							farSpotLight = spotLightsVector[selected];
						}
					}
				}
			}
		}

		program = App->programs->phongPbrProgram;
		glUseProgram(program);

		glUniform3fv(glGetUniformLocation(program, "diffuseColor"), 1, material.diffuseColor.ptr());
		glUniform3fv(glGetUniformLocation(program, "specularColor"), 1, material.specularColor.ptr());
		glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);

		int hasDiffuseMap = (material.hasDiffuseMap) ? 1 : 0;
		int hasSpecularMap = (material.hasSpecularMap) ? 1 : 0;
		int hasShininessInAlphaChannel = (material.hasShininessInAlphaChannel) ? 1 : 0;
		glUniform1i(glGetUniformLocation(program, "hasDiffuseMap"), hasDiffuseMap);
		glUniform1i(glGetUniformLocation(program, "hasSpecularMap"), hasSpecularMap);
		glUniform1i(glGetUniformLocation(program, "hasShininessInSpecularAlpha"), hasShininessInAlphaChannel);

		glUniform3fv(glGetUniformLocation(program, "light.ambient.color"), 1, App->renderer->ambientColor.ptr());

		if (directionalLight != nullptr) {
			glUniform3fv(glGetUniformLocation(program, "light.directional.direction"), 1, directionalLight->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.directional.color"), 1, directionalLight->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.directional.intensity"), directionalLight->intensity);
		}
		glUniform1i(glGetUniformLocation(program, "light.directional.isActive"), directionalLight ? 1 : 0);

		if (pointLightsVector.size() > 0) {
			glUniform3fv(glGetUniformLocation(program, "light.points[0].pos"), 1, pointLightsVector[0]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[0].color"), 1, pointLightsVector[0]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[0].intensity"), pointLightsVector[0]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[0].kc"), pointLightsVector[0]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[0].kl"), pointLightsVector[0]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[0].kq"), pointLightsVector[0]->kq);
		}
		if (pointLightsVector.size() > 1) {
			glUniform3fv(glGetUniformLocation(program, "light.points[1].pos"), 1, pointLightsVector[1]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[1].color"), 1, pointLightsVector[1]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[1].intensity"), pointLightsVector[1]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[1].kc"), pointLightsVector[1]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[1].kl"), pointLightsVector[1]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[1].kq"), pointLightsVector[1]->kq);
		}
		if (pointLightsVector.size() > 2) {
			glUniform3fv(glGetUniformLocation(program, "light.points[2].pos"), 1, pointLightsVector[2]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[2].color"), 1, pointLightsVector[2]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[2].intensity"), pointLightsVector[2]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[2].kc"), pointLightsVector[2]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[2].kl"), pointLightsVector[2]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[2].kq"), pointLightsVector[2]->kq);
		}
		if (pointLightsVector.size() > 3) {
			glUniform3fv(glGetUniformLocation(program, "light.points[3].pos"), 1, pointLightsVector[3]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[3].color"), 1, pointLightsVector[3]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[3].intensity"), pointLightsVector[3]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[3].kc"), pointLightsVector[3]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[3].kl"), pointLightsVector[3]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[3].kq"), pointLightsVector[3]->kq);
		}
		if (pointLightsVector.size() > 4) {
			glUniform3fv(glGetUniformLocation(program, "light.points[4].pos"), 1, pointLightsVector[4]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[4].color"), 1, pointLightsVector[4]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[4].intensity"), pointLightsVector[4]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[4].kc"), pointLightsVector[4]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[4].kl"), pointLightsVector[4]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[4].kq"), pointLightsVector[4]->kq);
		}
		if (pointLightsVector.size() > 5) {
			glUniform3fv(glGetUniformLocation(program, "light.points[5].pos"), 1, pointLightsVector[5]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[5].color"), 1, pointLightsVector[5]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[5].intensity"), pointLightsVector[5]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[5].kc"), pointLightsVector[5]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[5].kl"), pointLightsVector[5]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[5].kq"), pointLightsVector[5]->kq);
		}
		if (pointLightsVector.size() > 6) {
			glUniform3fv(glGetUniformLocation(program, "light.points[6].pos"), 1, pointLightsVector[6]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[6].color"), 1, pointLightsVector[6]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[6].intensity"), pointLightsVector[6]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[6].kc"), pointLightsVector[6]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[6].kl"), pointLightsVector[6]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[6].kq"), pointLightsVector[6]->kq);
		}
		if (pointLightsVector.size() > 7) {
			glUniform3fv(glGetUniformLocation(program, "light.points[7].pos"), 1, pointLightsVector[7]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[7].color"), 1, pointLightsVector[7]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[7].intensity"), pointLightsVector[7]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[7].kc"), pointLightsVector[7]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[7].kl"), pointLightsVector[7]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[7].kq"), pointLightsVector[7]->kq);
		}
		glUniform1i(glGetUniformLocation(program, "light.numPoints"), pointLightsVector.size());

		if (spotLightsVector.size() > 0) {
			glUniform3fv(glGetUniformLocation(program, "light.spots[0].pos"), 1, spotLightsVector[0]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[0].direction"), 1, spotLightsVector[0]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[0].color"), 1, spotLightsVector[0]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[0].intensity"), spotLightsVector[0]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[0].kc"), spotLightsVector[0]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[0].kl"), spotLightsVector[0]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[0].kq"), spotLightsVector[0]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[0].innerAngle"), spotLightsVector[0]->innerAngle);
			glUniform1f(glGetUniformLocation(program, "light.spots[0].outerAngle"), spotLightsVector[0]->outerAngle);
		}
		if (spotLightsVector.size() > 1) {
			glUniform3fv(glGetUniformLocation(program, "light.spots[1].pos"), 1, spotLightsVector[1]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[1].direction"), 1, spotLightsVector[1]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[1].color"), 1, spotLightsVector[1]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[1].intensity"), spotLightsVector[1]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[1].kc"), spotLightsVector[1]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[1].kl"), spotLightsVector[1]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[1].kq"), spotLightsVector[1]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[1].innerAngle"), spotLightsVector[1]->innerAngle);
			glUniform1f(glGetUniformLocation(program, "light.spots[1].outerAngle"), spotLightsVector[1]->outerAngle);
		}
		if (spotLightsVector.size() > 2) {
			glUniform3fv(glGetUniformLocation(program, "light.spots[2].pos"), 1, spotLightsVector[2]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[2].direction"), 1, spotLightsVector[2]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[2].color"), 1, spotLightsVector[2]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[2].intensity"), spotLightsVector[2]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[2].kc"), spotLightsVector[2]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[2].kl"), spotLightsVector[2]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[2].kq"), spotLightsVector[2]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[2].innerAngle"), spotLightsVector[2]->innerAngle);
			glUniform1f(glGetUniformLocation(program, "light.spots[2].outerAngle"), spotLightsVector[2]->outerAngle);
		}
		if (spotLightsVector.size() > 3) {
			glUniform3fv(glGetUniformLocation(program, "light.spots[3].pos"), 1, spotLightsVector[3]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[3].direction"), 1, spotLightsVector[3]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[3].color"), 1, spotLightsVector[3]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[3].intensity"), spotLightsVector[3]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[3].kc"), spotLightsVector[3]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[3].kl"), spotLightsVector[3]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[3].kq"), spotLightsVector[3]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[3].innerAngle"), spotLightsVector[3]->innerAngle);
			glUniform1f(glGetUniformLocation(program, "light.spots[3].outerAngle"), spotLightsVector[3]->outerAngle);
		}
		if (spotLightsVector.size() > 4) {
			glUniform3fv(glGetUniformLocation(program, "light.spots[4].pos"), 1, spotLightsVector[4]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[4].direction"), 1, spotLightsVector[4]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[4].color"), 1, spotLightsVector[4]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[4].intensity"), spotLightsVector[4]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[4].kc"), spotLightsVector[4]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[4].kl"), spotLightsVector[4]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[4].kq"), spotLightsVector[4]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[4].innerAngle"), spotLightsVector[4]->innerAngle);
			glUniform1f(glGetUniformLocation(program, "light.spots[4].outerAngle"), spotLightsVector[4]->outerAngle);
		}
		if (spotLightsVector.size() > 5) {
			glUniform3fv(glGetUniformLocation(program, "light.spots[5].pos"), 1, spotLightsVector[5]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[5].direction"), 1, spotLightsVector[5]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[5].color"), 1, spotLightsVector[5]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[5].intensity"), spotLightsVector[5]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[5].kc"), spotLightsVector[5]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[5].kl"), spotLightsVector[5]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[5].kq"), spotLightsVector[5]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[5].innerAngle"), spotLightsVector[5]->innerAngle);
			glUniform1f(glGetUniformLocation(program, "light.spots[5].outerAngle"), spotLightsVector[5]->outerAngle);
		}
		if (spotLightsVector.size() > 6) {
			glUniform3fv(glGetUniformLocation(program, "light.spots[6].pos"), 1, spotLightsVector[6]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[6].direction"), 1, spotLightsVector[6]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[6].color"), 1, spotLightsVector[6]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[6].intensity"), spotLightsVector[6]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[6].kc"), spotLightsVector[6]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[6].kl"), spotLightsVector[6]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[6].kq"), spotLightsVector[6]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[6].innerAngle"), spotLightsVector[6]->innerAngle);
			glUniform1f(glGetUniformLocation(program, "light.spots[6].outerAngle"), spotLightsVector[6]->outerAngle);
		}
		if (spotLightsVector.size() > 7) {
			glUniform3fv(glGetUniformLocation(program, "light.spots[7].pos"), 1, spotLightsVector[7]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[7].direction"), 1, spotLightsVector[7]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[7].color"), 1, spotLightsVector[7]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[7].intensity"), spotLightsVector[7]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[7].kc"), spotLightsVector[7]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[7].kl"), spotLightsVector[7]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[7].kq"), spotLightsVector[7]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[7].innerAngle"), spotLightsVector[7]->innerAngle);
			glUniform1f(glGetUniformLocation(program, "light.spots[7].outerAngle"), spotLightsVector[7]->outerAngle);
		}
		glUniform1i(glGetUniformLocation(program, "light.numSpots"), spotLightsVector.size());

		glUniform3fv(glGetUniformLocation(program, "viewPos"), 1, App->camera->GetPosition().ptr());
	} else {
		glUseProgram(program);
	}

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, modelMatrix.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, viewMatrix.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, projMatrix.ptr());
	glUniform1i(glGetUniformLocation(program, "diffuseMap"), 0);
	glUniform1i(glGetUniformLocation(program, "specularMap"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glTextureDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, glTextureSpecular);

	glBindVertexArray(mesh->vao);
	glDrawElements(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void ComponentMeshRenderer::SkinningCPU() {
	// Bind VBO buffer to iterate over vertex information
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

	float* vertices = (float*) glMapBufferRange(GL_ARRAY_BUFFER, 0, mesh->numVertices * 8, GL_MAP_WRITE_BIT);

	for (unsigned i = 0, vtxcount = 0; vtxcount < mesh->numVertices; i += 8, ++vtxcount) {
		float4 position(mesh->vertices[i + 0], mesh->vertices[i + 1], mesh->vertices[i + 2], 1);
		float4 normal(mesh->vertices[i + 3], mesh->vertices[i + 4], mesh->vertices[i + 5], 1);

		Mesh::Attach attachedInformation = mesh->attaches[vtxcount];

		float4 resPosition = float4::zero;
		float4 resNormal = float4::zero;

		for (unsigned j = 0; j < attachedInformation.numBones; ++j) {
			Mesh::Bone bone = mesh->bones[attachedInformation.bones[j]];
			float weightJ = attachedInformation.weights[j];

			const float4x4& inverseOffsetMatrix = bone.transform;

			// In this case, I can assume that the bone name is already stored in the map, so the find is not necessary
			const float4x4& boneTransform = goBones[bone.boneName]->GetComponent<ComponentTransform>()->GetGlobalMatrix();
			const float4x4& invertedParentModelTransform = GetOwner().GetParent()->GetComponent<ComponentTransform>()->GetGlobalMatrix().Inverted();

			float4x4 boneGameObjectTransform = invertedParentModelTransform * boneTransform;

			resPosition += weightJ * (boneGameObjectTransform * (inverseOffsetMatrix * position));
			//boneGameObjectTransform.InverseTranspose();
			resNormal += weightJ * (boneGameObjectTransform * (inverseOffsetMatrix * normal));
		}

		// Position
		vertices[i + 0] = resPosition.x;
		vertices[i + 1] = resPosition.y;
		vertices[i + 2] = resPosition.z;

		// Normal
		vertices[i + 3] = resNormal.x;
		vertices[i + 4] = resNormal.y;
		vertices[i + 5] = resNormal.z;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}