#include "ComponentMeshRenderer.h"

#include <windows.h>
#include <commdlg.h>
#include <direct.h>

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
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleEditor.h"

#include "assimp/mesh.h"
#include "GL/glew.h"
#include <imgui_internal.h>
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
				material.hasDiffuseMap = true;
				break;
			case TextureType::SPECULAR_MAP:
				material.specularMap = texture;
				material.hasSpecularMap = true;
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
	if (ImGui::CollapsingHeader("Mesh")) {
		bool active = IsActive();

		if (ImGui::Checkbox("Active", &active)) {
			active ? Enable() : Disable();
		}
		ImGui::SameLine();

		if (ImGui::Button("Remove")) {
			// TODO: Add delete Component tool
		}
		ImGui::Separator();

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
		ImGui::Separator();
	}

	if (ImGui::CollapsingHeader("Material")) {
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
			ImGui::BeginColumns("##material", 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			//First Column with the text names of widgets
			{
				//Diffuse
				{
					DrawFrame(1, material.diffuseMap?material.diffuseMap->glTexture:0, TextureType::DIFFUSE_MAP);
					ImGui::SameLine();
					ImGui::Text("Diffuse");
					if (ImGui::Button("No map##diffuse")) {
						material.diffuseMap = nullptr;
						material.hasDiffuseMap = false;
					}
				}
				//Specular
				{
					DrawFrame(2, material.specularMap?material.specularMap->glTexture:0, TextureType::SPECULAR_MAP);
					ImGui::SameLine();
					ImGui::Text("Specular");
					if (ImGui::Button("No map##diffuse")) {
						material.specularMap = nullptr;
						material.hasSpecularMap = false;
					}
				}

				ImGui::Text("Smoothness");

				//Normal
				{
					DrawFrame(3, 0, TextureType::NORMAL_MAP);
					ImGui::SameLine();
					ImGui::Text("Normal");
				}
			}

			ImGui::NextColumn();
			//Second column with the widgets
			{
				std::string id_cd("##color_d");
				//id_c.append();
				ImGui::PushID(id_cd.c_str());
				ImGui::ColorEdit3("", &material.diffuseColor[0], ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
				ImGui::PopID();

				ImGui::NewLine();
				ImGui::NewLine();

				std::string id_cs("##color_s");
				ImGui::PushID(id_cs.c_str());
				ImGui::ColorEdit3("", &material.specularColor[0], ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
				ImGui::PopID();

				ImGui::NewLine();
				ImGui::NewLine();

				std::string id_s("##smooth_");
				//id_s.append(_uuid);
				ImGui::PushID(id_s.c_str());
				ImGui::SliderFloat(id_s.c_str(), &material.smoothness, 0, 1);
				ImGui::PopID();
			}
			ImGui::EndColumns();

			// Shininess Combo
			const char* shininessItems[] = {"Shininess Value", "Specular Alpha", "Diffuse Alpha"};
			const char* shininessItemCurrent = shininessItems[material.hasShininessInAlphaChannel];
			if (ImGui::BeginCombo("##shininess", shininessItemCurrent)) {
				for (int n = 0; n < IM_ARRAYSIZE(shininessItems); ++n) {
					bool isSelected = (shininessItemCurrent == shininessItems[n]);
					if (ImGui::Selectable(shininessItems[n], isSelected)) {
						material.hasShininessInAlphaChannel = n;
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

		if (ImGui::BeginTabBar("Textures")) {
			if (material.diffuseMap != nullptr) {
				if (ImGui::BeginTabItem("Diffuse##map")) {
					ImGui::TextColored(App->editor->titleColor, "Diffuse Texture");
					ImGui::TextWrapped("Size:");
					ImGui::SameLine();
					int width;
					int height;
					glGetTextureLevelParameteriv(material.diffuseMap->glTexture, 0, GL_TEXTURE_WIDTH, &width);
					glGetTextureLevelParameteriv(material.diffuseMap->glTexture, 0, GL_TEXTURE_HEIGHT, &height);
					ImGui::TextWrapped("%d x %d", width, height);
					ImGui::Image((void*) material.diffuseMap->glTexture, ImVec2(200, 200));
					ImGui::EndTabItem();
				}
			}
			if (material.specularMap != nullptr) {
				if (ImGui::BeginTabItem("Specular##map")) {
					ImGui::TextColored(App->editor->titleColor, "Specular Texture");
					ImGui::TextWrapped("Size:");
					ImGui::SameLine();
					int width;
					int height;
					glGetTextureLevelParameteriv(material.specularMap->glTexture, 0, GL_TEXTURE_WIDTH, &width);
					glGetTextureLevelParameteriv(material.specularMap->glTexture, 0, GL_TEXTURE_HEIGHT, &height);
					ImGui::TextWrapped("%d x %d", width, height);
					ImGui::Image((void*) material.specularMap->glTexture, ImVec2(200, 200));
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
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

void ComponentMeshRenderer::Draw(const float4x4& modelMatrix) const {
	if (!IsActive()) return;

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
		int hasShininessInAlphaChannel = material.hasShininessInAlphaChannel;
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
