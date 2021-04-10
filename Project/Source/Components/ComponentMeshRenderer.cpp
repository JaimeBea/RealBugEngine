#include "ComponentMeshRenderer.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "Utils/ImGuiUtils.h"
#include "FileSystem/TextureImporter.h"
#include "GameObject.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceShader.h"
#include "Resources/ResourceTexture.h"
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

#define JSON_TAG_MESH_ID "MeshID"
#define JSON_TAG_MATERIAL_ID "MaterialID"

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

	UID oldMeshId = meshId;
	ImGui::ResourceSlot<ResourceMesh>("Mesh", &meshId);
	if (oldMeshId != meshId) {
		ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
		transform->InvalidateHierarchy();
	}
	ImGui::ResourceSlot<ResourceMaterial>("Material", &materialId);

	if (ImGui::TreeNode("Mesh")) {
		ResourceMesh* mesh = (ResourceMesh*) App->resources->GetResource(meshId);
		if (mesh != nullptr) {
			ImGui::TextColored(App->editor->titleColor, "Geometry");
			ImGui::TextWrapped("Num Vertices: ");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%d", mesh->numVertices);
			ImGui::TextWrapped("Num Triangles: ");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%d", mesh->numIndices / 3);
			ImGui::Separator();
			ImGui::TextColored(App->editor->titleColor, "Bounding Box");

			ImGui::Checkbox("Draw", &bbActive);
			if (bbActive) {
				ComponentBoundingBox* boundingBox = GetOwner().GetComponent<ComponentBoundingBox>();
				boundingBox->DrawBoundingBox();
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Material")) {
		ResourceMaterial* material = (ResourceMaterial*) App->resources->GetResource(materialId);
		if (material != nullptr) {
			ImGui::ResourceSlot<ResourceShader>("Shader", &material->shaderId);

			ResourceShader* shader = (ResourceShader*) App->resources->GetResource(material->shaderId);
			if (shader != nullptr) {
				if (shader->GetShaderType() == ShaderType::PHONG) {
					// Diffuse Texture Combo
					const char* diffuseItems[] = {"Diffuse Color", "Diffuse Texture"};
					const char* diffuseItemCurrent = diffuseItems[material->hasDiffuseMap];
					ImGui::TextColored(App->editor->textColor, "Diffuse Settings:");
					if (ImGui::BeginCombo("##diffuse", diffuseItemCurrent)) {
						for (int n = 0; n < IM_ARRAYSIZE(diffuseItems); ++n) {
							bool isSelected = (diffuseItemCurrent == diffuseItems[n]);
							if (ImGui::Selectable(diffuseItems[n], isSelected)) {
								material->hasDiffuseMap = n ? 1 : 0;
							}
							if (isSelected) {
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
					if (diffuseItemCurrent == diffuseItems[0]) {
						ImGui::ColorEdit3("Color##diffuse", material->diffuseColor.ptr());
					} else {
						ImGui::ResourceSlot<ResourceTexture>("Diffuse Texture", &material->diffuseMapId);
					}
					ImGui::Text("");

					// Specular Texture Combo
					const char* specularItems[] = {"Specular Color", "Specular Texture"};
					const char* specularItemCurrent = specularItems[material->hasSpecularMap];
					ImGui::TextColored(App->editor->textColor, "Specular Settings:");
					if (ImGui::BeginCombo("##specular", specularItemCurrent)) {
						for (int n = 0; n < IM_ARRAYSIZE(specularItems); ++n) {
							bool isSelected = (specularItemCurrent == specularItems[n]);
							if (ImGui::Selectable(specularItems[n], isSelected)) {
								material->hasSpecularMap = n ? 1 : 0;
							};
							if (isSelected) {
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
					if (specularItemCurrent == specularItems[0]) {
						ImGui::ColorEdit3("Color##specular", material->specularColor.ptr());
					} else {
						ImGui::ResourceSlot<ResourceTexture>("Specular Texture", &material->specularMapId);
					}

					// Smoothness Combo
					const char* smoothnessItems[] = {"Smoothness Value", "Smoothness Alpha"};
					const char* smoothnessItemCurrent = smoothnessItems[material->hasSmoothnessInAlphaChannel];
					if (ImGui::BeginCombo("##smoothness", smoothnessItemCurrent)) {
						for (int n = 0; n < IM_ARRAYSIZE(smoothnessItems); ++n) {
							bool isSelected = (smoothnessItemCurrent == smoothnessItems[n]);
							if (ImGui::Selectable(smoothnessItems[n], isSelected)) {
								material->hasSmoothnessInAlphaChannel = n ? 1 : 0;
							}
							if (isSelected) {
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
					if (smoothnessItemCurrent == smoothnessItems[0]) {
						ImGui::DragFloat("Smoothness##smoothness", &material->smoothness, App->editor->dragSpeed3f, 0.0f, 1000.0f);
					}
				}
				ImGui::Separator();
				ImGui::TextColored(App->editor->titleColor, "Filters");

				/*
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
				if (material->diffuseMap != nullptr) {
					ImGui::TextColored(App->editor->titleColor, "Diffuse Texture");
					ImGui::TextWrapped("Size:##diffuse");
					ImGui::SameLine();
					int width;
					int height;
					glGetTextureLevelParameteriv(material->diffuseMap->glTexture, 0, GL_TEXTURE_WIDTH, &width);
					glGetTextureLevelParameteriv(material->diffuseMap->glTexture, 0, GL_TEXTURE_HEIGHT, &height);
					ImGui::TextWrapped("%d x %d##diffuse", width, height);
					ImGui::Image((void*) material->diffuseMap->glTexture, ImVec2(200, 200));
					ImGui::Separator();
				}
				if (material->specularMap != nullptr) {
					ImGui::TextColored(App->editor->titleColor, "Specular Texture");
					ImGui::TextWrapped("Size:##specular");
					ImGui::SameLine();
					int width;
					int height;
					glGetTextureLevelParameteriv(material->specularMap->glTexture, 0, GL_TEXTURE_WIDTH, &width);
					glGetTextureLevelParameteriv(material->specularMap->glTexture, 0, GL_TEXTURE_HEIGHT, &height);
					ImGui::TextWrapped("%d x %d##specular", width, height);
					ImGui::Image((void*) material->specularMap->glTexture, ImVec2(200, 200));
					ImGui::Separator();
				}
				*/
			}
		}
		ImGui::TreePop();
	}
}

void ComponentMeshRenderer::Init() {
	ResourceMesh* mesh = static_cast<ResourceMesh*>(App->resources->GetResource(meshId));
	if (!mesh) return;

	palette.resize(mesh->numBones);
	for (unsigned i = 0; i < mesh->numBones; ++i) {
		palette[i] = float4x4::identity;
	}
}

void ComponentMeshRenderer::Update() {
	ResourceMesh* mesh = static_cast<ResourceMesh*>(App->resources->GetResource(meshId));
	if (!mesh) return;

	if (App->time->GetDeltaTime() <= 0) return;

	const GameObject* parent = GetOwner().GetParent();
	const GameObject* rootBone = parent->GetRootBone();
	if (rootBone != nullptr) {
		const GameObject* rootBoneParent = parent->GetRootBone()->GetParent();
		const float4x4& invertedRootBoneTransform = (rootBoneParent && rootBoneParent != parent) ? rootBoneParent->GetComponent<ComponentTransform>()->GetGlobalMatrix().Inverted() : float4x4::identity;

		for (unsigned i = 0; i < mesh->numBones; ++i) {
			const GameObject* bone = goBones.at(mesh->bones[i].boneName);
			palette[i] = invertedRootBoneTransform * bone->GetComponent<ComponentTransform>()->GetGlobalMatrix() * mesh->bones[i].transform;
		}
	}
}

void ComponentMeshRenderer::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_MESH_ID] = meshId;
	jComponent[JSON_TAG_MATERIAL_ID] = materialId;
}

void ComponentMeshRenderer::Load(JsonValue jComponent) {
	meshId = jComponent[JSON_TAG_MESH_ID];
	if (meshId != 0) App->resources->IncreaseReferenceCount(meshId);
	materialId = jComponent[JSON_TAG_MATERIAL_ID];
	if (materialId != 0) App->resources->IncreaseReferenceCount(materialId);
}

void ComponentMeshRenderer::DuplicateComponent(GameObject& owner) {
	ComponentMeshRenderer* component = owner.CreateComponent<ComponentMeshRenderer>();
	component->meshId = meshId;
	component->materialId = materialId;
}

void ComponentMeshRenderer::Draw(const float4x4& modelMatrix) const {
	if (!IsActiveInHierarchy()) return;

	ResourceMesh* mesh = (ResourceMesh*) App->resources->GetResource(meshId);
	if (mesh == nullptr) return;

	ResourceMaterial* material = (ResourceMaterial*) App->resources->GetResource(materialId);
	if (material == nullptr) return;

	ResourceShader* shader = (ResourceShader*) App->resources->GetResource(material->shaderId);
	if (shader == nullptr) return;

	unsigned program = shader->GetShaderProgram();

	float4x4 viewMatrix = App->camera->GetViewMatrix();
	float4x4 projMatrix = App->camera->GetProjectionMatrix();
	unsigned glTextureDiffuse = 0;
	unsigned glTextureSpecular = 0;

	ResourceTexture* diffuse = (ResourceTexture*) App->resources->GetResource(material->diffuseMapId);
	glTextureDiffuse = diffuse ? diffuse->glTexture : 0;
	ResourceTexture* specular = (ResourceTexture*) App->resources->GetResource(material->specularMapId);
	glTextureSpecular = specular ? specular->glTexture : 0;

	ComponentLight* directionalLight = nullptr;
	ComponentLight* pointLightsArray[8];
	float pointDistancesArray[8];
	unsigned pointLightsArraySize = 0;
	ComponentLight* spotLightsArray[8];
	float spotDistancesArray[8];
	unsigned spotLightsArraySize = 0;

	if (shader->GetShaderType() == ShaderType::PHONG) {
		float farPointDistance = 0;
		ComponentLight* farPointLight = nullptr;
		float farSpotDistance = 0;
		ComponentLight* farSpotLight = nullptr;

		for (ComponentLight& light : GetOwner().scene->lightComponents) {
			if (light.lightType == LightType::DIRECTIONAL) {
				// It takes the first actived Directional Light inside the Pool
				if (light.IsActiveInHierarchy() && directionalLight == nullptr) {
					directionalLight = &light;
					continue;
				}
			} else if (light.lightType == LightType::POINT) {
				if (light.IsActiveInHierarchy()) {
					float3 meshPosition = GetOwner().GetComponent<ComponentTransform>()->GetPosition();
					float3 lightPosition = light.GetOwner().GetComponent<ComponentTransform>()->GetPosition();
					float distance = Distance(meshPosition, lightPosition);
					if (pointLightsArraySize < 8) {
						pointDistancesArray[pointLightsArraySize] = distance;
						pointLightsArray[pointLightsArraySize] = &light;
						pointLightsArraySize += 1;

						if (distance > farPointDistance) {
							farPointLight = &light;
							farPointDistance = distance;
						}
					} else {
						if (distance < farPointDistance) {
							int count = 0;
							int selected = -1;
							for (float pointDistance : pointDistancesArray) {
								if (pointDistance == farPointDistance) selected = count;
								count += 1;
							}

							pointLightsArray[selected] = &light;
							pointDistancesArray[selected] = distance;

							count = 0;
							selected = -1;
							float maxDistance = 0;
							for (float pointDistance : pointDistancesArray) {
								if (pointDistance > maxDistance) {
									maxDistance = pointDistance;
									selected = count;
								}
								count += 1;
							}

							farPointDistance = maxDistance;
							farPointLight = pointLightsArray[selected];
						}
					}
				}
			} else if (light.lightType == LightType::SPOT) {
				if (light.IsActiveInHierarchy()) {
					float3 meshPosition = GetOwner().GetComponent<ComponentTransform>()->GetPosition();
					float3 lightPosition = light.GetOwner().GetComponent<ComponentTransform>()->GetPosition();
					float distance = Distance(meshPosition, lightPosition);
					if (spotLightsArraySize < 8) {
						spotDistancesArray[spotLightsArraySize] = distance;
						spotLightsArray[spotLightsArraySize] = &light;
						spotLightsArraySize += 1;

						if (distance > farSpotDistance) {
							farSpotLight = &light;
							farSpotDistance = distance;
						}
					} else {
						if (distance < farSpotDistance) {
							int count = 0;
							int selected = -1;
							for (float spotDistance : spotDistancesArray) {
								if (spotDistance == farSpotDistance) selected = count;
								count += 1;
							}

							spotLightsArray[selected] = &light;
							spotDistancesArray[selected] = distance;

							count = 0;
							selected = -1;
							float maxDistance = 0;
							for (float spotDistance : spotDistancesArray) {
								if (spotDistance > maxDistance) {
									maxDistance = spotDistance;
									selected = count;
								}
								count += 1;
							}

							farSpotDistance = maxDistance;
							farSpotLight = spotLightsArray[selected];
						}
					}
				}
			}
		}

		glUseProgram(program);

		glUniform3fv(glGetUniformLocation(program, "diffuseColor"), 1, material->diffuseColor.ptr());
		glUniform3fv(glGetUniformLocation(program, "specularColor"), 1, material->specularColor.ptr());
		int hasDiffuseMap = diffuse ? 1 : 0;
		int hasSpecularMap = specular ? 1 : 0;
		int hasShininessInAlphaChannel = (material->hasSmoothnessInAlphaChannel) ? 1 : 0;
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

		if (pointLightsArraySize > 0) {
			glUniform3fv(glGetUniformLocation(program, "light.points[0].pos"), 1, pointLightsArray[0]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[0].color"), 1, pointLightsArray[0]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[0].intensity"), pointLightsArray[0]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[0].kc"), pointLightsArray[0]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[0].kl"), pointLightsArray[0]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[0].kq"), pointLightsArray[0]->kq);
		}
		if (pointLightsArraySize > 1) {
			glUniform3fv(glGetUniformLocation(program, "light.points[1].pos"), 1, pointLightsArray[1]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[1].color"), 1, pointLightsArray[1]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[1].intensity"), pointLightsArray[1]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[1].kc"), pointLightsArray[1]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[1].kl"), pointLightsArray[1]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[1].kq"), pointLightsArray[1]->kq);
		}
		if (pointLightsArraySize > 2) {
			glUniform3fv(glGetUniformLocation(program, "light.points[2].pos"), 1, pointLightsArray[2]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[2].color"), 1, pointLightsArray[2]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[2].intensity"), pointLightsArray[2]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[2].kc"), pointLightsArray[2]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[2].kl"), pointLightsArray[2]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[2].kq"), pointLightsArray[2]->kq);
		}
		if (pointLightsArraySize > 3) {
			glUniform3fv(glGetUniformLocation(program, "light.points[3].pos"), 1, pointLightsArray[3]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[3].color"), 1, pointLightsArray[3]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[3].intensity"), pointLightsArray[3]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[3].kc"), pointLightsArray[3]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[3].kl"), pointLightsArray[3]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[3].kq"), pointLightsArray[3]->kq);
		}
		if (pointLightsArraySize > 4) {
			glUniform3fv(glGetUniformLocation(program, "light.points[4].pos"), 1, pointLightsArray[4]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[4].color"), 1, pointLightsArray[4]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[4].intensity"), pointLightsArray[4]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[4].kc"), pointLightsArray[4]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[4].kl"), pointLightsArray[4]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[4].kq"), pointLightsArray[4]->kq);
		}
		if (pointLightsArraySize > 5) {
			glUniform3fv(glGetUniformLocation(program, "light.points[5].pos"), 1, pointLightsArray[5]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[5].color"), 1, pointLightsArray[5]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[5].intensity"), pointLightsArray[5]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[5].kc"), pointLightsArray[5]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[5].kl"), pointLightsArray[5]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[5].kq"), pointLightsArray[5]->kq);
		}
		if (pointLightsArraySize > 6) {
			glUniform3fv(glGetUniformLocation(program, "light.points[6].pos"), 1, pointLightsArray[6]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[6].color"), 1, pointLightsArray[6]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[6].intensity"), pointLightsArray[6]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[6].kc"), pointLightsArray[6]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[6].kl"), pointLightsArray[6]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[6].kq"), pointLightsArray[6]->kq);
		}
		if (pointLightsArraySize > 7) {
			glUniform3fv(glGetUniformLocation(program, "light.points[7].pos"), 1, pointLightsArray[7]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.points[7].color"), 1, pointLightsArray[7]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.points[7].intensity"), pointLightsArray[7]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.points[7].kc"), pointLightsArray[7]->kc);
			glUniform1f(glGetUniformLocation(program, "light.points[7].kl"), pointLightsArray[7]->kl);
			glUniform1f(glGetUniformLocation(program, "light.points[7].kq"), pointLightsArray[7]->kq);
		}
		glUniform1i(glGetUniformLocation(program, "light.numPoints"), pointLightsArraySize);

		if (spotLightsArraySize > 0) {
			glUniform3fv(glGetUniformLocation(program, "light.spots[0].pos"), 1, spotLightsArray[0]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[0].direction"), 1, spotLightsArray[0]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[0].color"), 1, spotLightsArray[0]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[0].intensity"), spotLightsArray[0]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[0].kc"), spotLightsArray[0]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[0].kl"), spotLightsArray[0]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[0].kq"), spotLightsArray[0]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[0].innerAngle"), spotLightsArray[0]->innerAngle);
			glUniform1f(glGetUniformLocation(program, "light.spots[0].outerAngle"), spotLightsArray[0]->outerAngle);
		}
		if (spotLightsArraySize > 1) {
			glUniform3fv(glGetUniformLocation(program, "light.spots[1].pos"), 1, spotLightsArray[1]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[1].direction"), 1, spotLightsArray[1]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[1].color"), 1, spotLightsArray[1]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[1].intensity"), spotLightsArray[1]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[1].kc"), spotLightsArray[1]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[1].kl"), spotLightsArray[1]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[1].kq"), spotLightsArray[1]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[1].innerAngle"), spotLightsArray[1]->innerAngle);
			glUniform1f(glGetUniformLocation(program, "light.spots[1].outerAngle"), spotLightsArray[1]->outerAngle);
		}
		if (spotLightsArraySize > 2) {
			glUniform3fv(glGetUniformLocation(program, "light.spots[2].pos"), 1, spotLightsArray[2]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[2].direction"), 1, spotLightsArray[2]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[2].color"), 1, spotLightsArray[2]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[2].intensity"), spotLightsArray[2]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[2].kc"), spotLightsArray[2]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[2].kl"), spotLightsArray[2]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[2].kq"), spotLightsArray[2]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[2].innerAngle"), spotLightsArray[2]->innerAngle);
			glUniform1f(glGetUniformLocation(program, "light.spots[2].outerAngle"), spotLightsArray[2]->outerAngle);
		}
		if (spotLightsArraySize > 3) {
			glUniform3fv(glGetUniformLocation(program, "light.spots[3].pos"), 1, spotLightsArray[3]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[3].direction"), 1, spotLightsArray[3]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[3].color"), 1, spotLightsArray[3]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[3].intensity"), spotLightsArray[3]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[3].kc"), spotLightsArray[3]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[3].kl"), spotLightsArray[3]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[3].kq"), spotLightsArray[3]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[3].innerAngle"), spotLightsArray[3]->innerAngle);
			glUniform1f(glGetUniformLocation(program, "light.spots[3].outerAngle"), spotLightsArray[3]->outerAngle);
		}
		if (spotLightsArraySize > 4) {
			glUniform3fv(glGetUniformLocation(program, "light.spots[4].pos"), 1, spotLightsArray[4]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[4].direction"), 1, spotLightsArray[4]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[4].color"), 1, spotLightsArray[4]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[4].intensity"), spotLightsArray[4]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[4].kc"), spotLightsArray[4]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[4].kl"), spotLightsArray[4]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[4].kq"), spotLightsArray[4]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[4].innerAngle"), spotLightsArray[4]->innerAngle);
			glUniform1f(glGetUniformLocation(program, "light.spots[4].outerAngle"), spotLightsArray[4]->outerAngle);
		}
		if (spotLightsArraySize > 5) {
			glUniform3fv(glGetUniformLocation(program, "light.spots[5].pos"), 1, spotLightsArray[5]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[5].direction"), 1, spotLightsArray[5]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[5].color"), 1, spotLightsArray[5]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[5].intensity"), spotLightsArray[5]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[5].kc"), spotLightsArray[5]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[5].kl"), spotLightsArray[5]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[5].kq"), spotLightsArray[5]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[5].innerAngle"), spotLightsArray[5]->innerAngle);
			glUniform1f(glGetUniformLocation(program, "light.spots[5].outerAngle"), spotLightsArray[5]->outerAngle);
		}
		if (spotLightsArraySize > 6) {
			glUniform3fv(glGetUniformLocation(program, "light.spots[6].pos"), 1, spotLightsArray[6]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[6].direction"), 1, spotLightsArray[6]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[6].color"), 1, spotLightsArray[6]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[6].intensity"), spotLightsArray[6]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[6].kc"), spotLightsArray[6]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[6].kl"), spotLightsArray[6]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[6].kq"), spotLightsArray[6]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[6].innerAngle"), spotLightsArray[6]->innerAngle);
			glUniform1f(glGetUniformLocation(program, "light.spots[6].outerAngle"), spotLightsArray[6]->outerAngle);
		}
		if (spotLightsArraySize > 7) {
			glUniform3fv(glGetUniformLocation(program, "light.spots[7].pos"), 1, spotLightsArray[7]->pos.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[7].direction"), 1, spotLightsArray[7]->direction.ptr());
			glUniform3fv(glGetUniformLocation(program, "light.spots[7].color"), 1, spotLightsArray[7]->color.ptr());
			glUniform1f(glGetUniformLocation(program, "light.spots[7].intensity"), spotLightsArray[7]->intensity);
			glUniform1f(glGetUniformLocation(program, "light.spots[7].kc"), spotLightsArray[7]->kc);
			glUniform1f(glGetUniformLocation(program, "light.spots[7].kl"), spotLightsArray[7]->kl);
			glUniform1f(glGetUniformLocation(program, "light.spots[7].kq"), spotLightsArray[7]->kq);
			glUniform1f(glGetUniformLocation(program, "light.spots[7].innerAngle"), spotLightsArray[7]->innerAngle);
			glUniform1f(glGetUniformLocation(program, "light.spots[7].outerAngle"), spotLightsArray[7]->outerAngle);
		}
		glUniform1i(glGetUniformLocation(program, "light.numSpots"), spotLightsArraySize);

		glUniform3fv(glGetUniformLocation(program, "viewPos"), 1, App->camera->GetPosition().ptr());
	} else {
		glUseProgram(program);
	}

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, modelMatrix.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, viewMatrix.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, projMatrix.ptr());
	if (palette.size() > 0) {
		glUniformMatrix4fv(glGetUniformLocation(program, "palette"), palette.size(), GL_TRUE, palette[0].ptr());
	}
	glUniform1i(glGetUniformLocation(program, "hasBones"), goBones.size());

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