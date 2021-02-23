#include "FileDialog.h"

#include "imgui.h"
#include "Application.h"
#include "IconsForkAwesome.h"

#include <vector>
static char currentSelectedPath_[FILENAME_MAX];
static std::string title_;
static std::string currentDrive_;
static bool open_ = false;
static AllowedExtensionsFlag ext_;
static std::string selectedPath_;
static bool editMode_ = false;

void FileDialog::Init(const std::string& title, bool editMode, AllowedExtensionsFlag ext, const std::string& defaultPath) {
	title_ = title;
	if (defaultPath == "") {
		sprintf(currentSelectedPath_, App->files->GetAbsolutePath(".").c_str());
	} else {
		sprintf(currentSelectedPath_, defaultPath.c_str());
	}
	editMode_ = editMode;
	currentDrive_ = std::string() + currentSelectedPath_[0] + ":";
	ext_ = ext;
	open_ = true;
	ImGui::OpenPopup(title.c_str());
}

bool FileDialog::OpenDialog(std::string& selectedPath) {
	
	if (ImGui::BeginPopupModal("Load scene")) {
		ImGui::PushItemWidth(4 * ImGui::GetFontSize());
		if (ImGui::BeginCombo("##Drives", currentDrive_.c_str())) {
			std::vector<std::string> drives = App->files->GetDrives();
			for (std::string& drive : drives) {
				if (ImGui::Selectable(drive.c_str(), currentDrive_[0] == drive[0])) {
					currentDrive_ = drive;
					sprintf(currentSelectedPath_, drive.c_str());
				}
			}
			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::InputText("##route", currentSelectedPath_, FILENAME_MAX);
		ImGui::PopItemWidth();
		float reserveHeight = ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("FileExplorer", ImVec2(0, -reserveHeight), true);

		for (std::string& file : App->files->GetFilesInFolder(currentSelectedPath_, ext_)) {
			std::string icon = ICON_FK_FILE;
			std::string absoluteFilePath = std::string(currentSelectedPath_) + "\\" + file;
			bool isDirectory = App->files->IsDirectory(absoluteFilePath.c_str());
			if (isDirectory) icon = ICON_FK_FOLDER;

			std::string selectableLabel = std::string(icon + " ") + file;
			if (ImGui::Selectable(selectableLabel.c_str())) {
				if (isDirectory) {
					if ((selectedPath_ == absoluteFilePath)) {
						if (file == "..") {
							sprintf(currentSelectedPath_, App->files->UpOneLevel(currentSelectedPath_).c_str());
						} else {
							sprintf(currentSelectedPath_, absoluteFilePath.c_str());
						}
						selectedPath_ = "";
					} else {
						selectedPath_ = absoluteFilePath.c_str();
					}
				} else {
					selectedPath_ = absoluteFilePath.c_str();
				}
				
			}
		}
		selectedPath = selectedPath_;
		ImGui::EndChild();

		ImGui::LabelText("##selectedFile", selectedPath_.c_str(), FILENAME_MAX);
		ImGui::SameLine();
		if (ImGui::Button("Accept")) {
			ImGui::CloseCurrentPopup();
			open_ = false;
			return true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
			open_ = false;
		}
		ImGui::EndPopup();
	}
	return false;
}
