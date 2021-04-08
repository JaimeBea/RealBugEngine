#include "ComponentMeshRenderer.h"

#include <windows.h>
#include <commdlg.h>
#include <direct.h>

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
#include <imgui_internal.h>
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
#define JSON_TAG_SMOOTHNESS "Smoothness"
#define JSON_TAG_HAS_SMOOTHNESS_IN_ALPHA_CHANNEL "HasSmoothnessInAlphaChannel"
#define JSON_TAG_METALLIC_MAP_FILE_NAME "MetallicMapFileName"
#define JSON_TAG_METALLIC "Metallic"

// THIS FUNCTION WILL BE REPLACED WITH THE CORRESPONDING FROM FILESYSTEM
std::string OpenDialog(const char* _filters, const char* _dir, int* _size) {
	char path[MAX_PATH];
	_fullpath(path, NULL, MAX_PATH);
	char filename[MAX_PATH];
	OPENFILENAME ofn;
	HANDLE hf;

	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL; // If you have a window to center over, put its HANDLE here
	ofn.lpstrFile = filename;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = _filters; // "Text Files\0*.txt\0Any File\0*.*\0";
	ofn.lpstrTitle = "Select a File";
	ofn.lpstrInitialDir = _dir;
	ofn.Flags = OFN_DONTADDTORECENT | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn)) {
		hf = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES) NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
		LARGE_INTEGER lFileSize;
		GetFileSizeEx(hf, &lFileSize);
		*_size = lFileSize.QuadPart;
		CloseHandle(hf);
		_chdir(path);
		return filename;
	} else
		_chdir(path);
	return "\0";
}

void ComponentMeshRenderer::DrawFrame(int frameId, unsigned int texId, TextureType texType) {
	float2 image_size(24, 24);

	ImGui::BeginChildFrame(frameId, ImVec2(32, 32));

	float2 window_size(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
	ImVec2 cursorPos((window_size.x - image_size.x) * 0.5f, (window_size.y - image_size.y) * 0.5f);
	ImGui::SetCursorPos(cursorPos);
	ImGui::Image((void*) texId, ImVec2(image_size.x, image_size.y));
	if (ImGui::IsItemClicked()) {
		int size;
		std::string fn;
		// This will be substituted by the filesystem open dialog.
		fn = OpenDialog("Texture Files\0*.dds;", ".\\Library\\Textures\\", &size);
		if (!fn.empty()) {
			Texture* texture = App->resources->ObtainTexture();
			texture->fileName = fn.substr(fn.find_last_of('\\') + 1, fn.size() - fn.find_last_of('\\') - 5);
			LOG(texture->fileName.c_str())
			TextureImporter::LoadTexture(texture);

			switch (texType) {
			case TextureType::DIFFUSE_MAP:
				material.diffuseMap = texture;
				break;
			case TextureType::SPECULAR_MAP:
				material.specularMap = texture;
				break;
			case TextureType::METALLIC_MAP:
				material.metallicMap = texture;
				break;
			case TextureType::NORMAL_MAP:
				break;
			}
		}
	}

	// TODO: When we get a PanelExplorer, allow to drag and drop textures from there
	/* Example:
	if (ImGui::BeginDragDropTarget() && IsImage(pwd.extension().string())) {
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("explorer_drag_n_drop");
		if (payload) {
			//TODO: In a future --> The resource texture would be loaded, and then only
			// we need to change the resource texture
			// for now we load the texture via DevIL
			LOG(_INFO, "Loading texture: %s", pwd.filename().string());
			TextureInformation texture;
			App->texturer->LoadTexture(pwd.string().c_str(), texture);

			switch (flag) {
			case MatStandard_Flags_Albedo_Map:
				matSt->SetAlbedoMap(texture);
				break;
			case MatStandard_Flags_Specular_Map:
				matSt->SetSpecularMap(texture);
				break;
			case MatStandard_Flags_Normal_Map:
				matSt->SetNormalMap(texture);
				break;
			}
		}
		ImGui::EndDragDropTarget();

	}*/

	ImGui::EndChildFrame();
}

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

	for (unsigned i = 0; i < mesh->numBones; ++i) {
		const GameObject* bone = goBones.at(mesh->bones[i].boneName);
		// TODO: if(bone) might not be necessary
		if (bone) {
			const GameObject* parent = GetOwner().GetParent();
			const GameObject* rootBoneParent = parent->GetRootBone()->GetParent();
			const float4x4& invertedRootBoneTransform = (rootBoneParent && rootBoneParent != parent) ? rootBoneParent->GetComponent<ComponentTransform>()->GetGlobalMatrix().Inverted() : float4x4::identity;
			palette[i] = invertedRootBoneTransform * bone->GetComponent<ComponentTransform>()->GetGlobalMatrix() * mesh->bones[i].transform;
		} else {
			palette[i] = float4x4::identity;
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
	std::vector<ComponentLight*> pointLightsVector;
	std::vector<float> pointDistancesVector;
	std::vector<ComponentLight*> spotLightsVector;
	std::vector<float> spotDistancesVector;

	if (shader->GetShaderType() == ShaderType::PHONG) {
		float farPointDistance = 0;
		ComponentLight* farPointLight = nullptr;
		float farSpotDistance = 0;
		ComponentLight* farSpotLight = nullptr;

		for (GameObject& object : GetOwner().scene->gameObjects) {
			ComponentLight* light = object.GetComponent<ComponentLight>();
			if (light == nullptr) continue;

			if (light->lightType == LightType::DIRECTIONAL) {
				// It takes the first actived Directional Light inside the Pool
				if (light->IsActiveInHierarchy() && directionalLight == nullptr) {
					directionalLight = light;
					continue;
				}
			} else if (light->lightType == LightType::POINT) {
				if (light->IsActiveInHierarchy()) {
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
				if (light->IsActiveInHierarchy()) {
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

		glUseProgram(program);

		glUniform3fv(glGetUniformLocation(program, "diffuseColor"), 1, material->diffuseColor.ptr());
		glUniform3fv(glGetUniformLocation(program, "specularColor"), 1, material->specularColor.ptr());
		int hasDiffuseMap = diffuse ? 1 : 0;
		int hasSpecularMap = specular ? 1 : 0;
		int hasShininessInAlphaChannel = (material->hasSmoothnessInAlphaChannel) ? 1 : 0;
		glUniform1i(glGetUniformLocation(program, "hasDiffuseMap"), hasDiffuseMap);
		glUniform1i(glGetUniformLocation(program, "hasSpecularMap"), hasSpecularMap);
		glUniform1i(glGetUniformLocation(program, "hasShininessInSpecularAlpha"), hasShininessInAlphaChannel);

	// Lights uniforms setting
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