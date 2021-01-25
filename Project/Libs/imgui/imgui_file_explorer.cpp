#include "imgui_file_explorer.h"

#include <map>
#include <string>

#include "imgui_internal.h"

#include "IconsForkAwesome.h"
#include "IconsFontAwesome5.h"

static std::map<std::string, bool> lookupFolders;

static std::string selected_node = "Assets";
static std::string selected_entry = "";

// clang-format off
constexpr char* imageFormat[63] = {
	".blp", ".bmp", ".bw", ".cur", ".cut", ".dcm", ".dcx", ".dds", ".dicom", ".dpx", ".exr", ".fit",
	".fits", ".ftx", ".gif", ".h", ".hdp", ".hdr", ".icns", ".ico", ".iff", ".im", ".iwi", ".jng", ".jp2",
	".jpe", ".jpeg", ".jpg", ".lbm", ".lif", ".lmp", ".mdl", ".mng", ".mp3", ".pbm", ".pcd", ".pcx", ".pgm",
	".pic", ".pix", ".png", ".pnm", ".ppm", ".psd", ".psp", ".pxr", ".ras", ".rgb", ".rgba", ".rot", ".rs", ".sgi",
	".sun", ".texture", ".tga", ".tif", ".tiff", ".tpl", ".utx", ".vtf", ".wal", ".wdp", ".xpm" };

constexpr char* meshFormat[48] = { ".3d", ".3ds", ".ac", ".ase", ".b3d", ".blend", ".bvh", ".cob", ".csm",
".dae", ".dxf", ".fbx", ".glb", ".gltf", ".hmp", ".ifc", ".irr", ".irrmesh",
".lwo", ".lws", ".lxo", ".md2", ".md3", ".md5", ".mdc", ".mdl", ".mdl", ".ms3d",
".ndo", ".nff", ".nff", ".obj", ".off", ".ogex", ".pk3", ".ply", ".q3d", ".q3s",
".raw", ".scn", ".smd", ".stl", ".ter", ".vta", ".x", ".xgl", ".xml", ".zgl" };
// clang-format on

namespace ImGui {

	FileExplorer::FileExplorer() {
		std::filesystem::_Current_path(_pwd);
		_pwd += "\\Assets";
		lookupFolders["Assets"] = true;
	}

	void FileExplorer::DirectoryTreeViewRecursive(const std::filesystem::path& path, int& count) {
		ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			bool entryIsDir = std::filesystem::is_directory(entry.path());
			if (!entryIsDir) continue;

			ImGuiTreeNodeFlags node_flags = base_flags;

			std::string name = entry.path().string();

			auto lastSlash = name.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			name = name.substr(lastSlash, name.size() - lastSlash);

			bool is_selected = selected_node == name;
			if (is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;

			bool node_open = false;

			using fp = bool (*)(const std::filesystem::path&);
			int fileCount = std::count_if(
				std::filesystem::directory_iterator(entry.path()),
				std::filesystem::directory_iterator {},
				fp(std::filesystem::is_directory));

			if (fileCount > 0) {
				std::string wName;

				if (lookupFolders[name])
					wName = ICON_FK_FOLDER_OPEN " ";
				else if (std::filesystem::is_empty(entry.path()))
					wName = ICON_FK_FOLDER_O " "; //Find folder border or similar
				else
					wName = ICON_FK_FOLDER " ";

				wName.append(name);

				ImGui::SetNextItemOpen(lookupFolders[name]);

				node_open = ImGui::TreeNodeEx((void*) (intptr_t)(count), node_flags, wName.c_str());
			} else {
				std::string wName(std::filesystem::is_empty(entry.path()) ? ICON_FK_FOLDER_O " " : ICON_FK_FOLDER " ");
				wName.append(name);

				ImGui::TreeNodeEx((void*) (intptr_t)(count), node_flags | ImGuiTreeNodeFlags_Leaf, wName.c_str());
				ImGui::TreePop();
			}

			if (ImGui::IsItemClicked(0)) {
				selected_node = name;
				_pwd = entry.path();
			}

			count++;

			if (node_open) {
				lookupFolders[name] = true;
				DirectoryTreeViewRecursive(entry.path(), count);
				ImGui::TreePop();

			} else {
				lookupFolders[name] = false;
			}
		}
	}

	void FileExplorer::DrawFileTree(std::string directoryPath) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 {0.0f, 0.0f});

		int base_flags = ImGuiTreeNodeFlags_OpenOnArrow;

		static bool open = false;

		if (selected_node == directoryPath)
			base_flags |= ImGuiTreeNodeFlags_Selected;

		std::string wName((open ? ICON_FA_FOLDER_OPEN " " : ICON_FA_FOLDER " "));
		wName.append(directoryPath);

		open = ImGui::TreeNodeEx(directoryPath.c_str(), base_flags | ImGuiTreeNodeFlags_DefaultOpen, wName.c_str());

		int count = 0;

		if (ImGui::IsItemClicked(0)) {
			selected_node = directoryPath;
			_pwd = directoryPath;
		}

		count++;

		if (open) {
			DirectoryTreeViewRecursive(directoryPath, count);
			ImGui::TreePop();
		}

		ImGui::PopStyleVar();
	}

	bool IsImage(const std::string& extension) {
		for (int i = 0; i < IM_ARRAYSIZE(imageFormat); i++) {
			if (_strcmpi(imageFormat[i], extension.c_str()) == 0) return true;
		}
		return false;
	}

	void FileExplorer::ShowContent() {
		for (const auto& entry : std::filesystem::directory_iterator(_pwd)) {
			std::string name = entry.path().string();
			std::string label;

			auto lastSlash = name.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			name = name.substr(lastSlash, name.size() - lastSlash);

			bool selected = selected_entry == name;

			if (entry.is_directory()) {
				label = std::string(ICON_FK_FOLDER " ") + name;
			} else if (IsImage(entry.path().extension().string())) {
				label = std::string(ICON_FK_FILE_IMAGE_O " ") + name;
			} else
				label = std::string(ICON_FK_FILE " ") + name;

			if (ImGui::Selectable(label.c_str(), selected)) {
				selected_entry = name;
			}

			if (entry.is_directory() && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
				selected_node = name;
				_pwd = entry.path();
				lookupFolders[name] = true;
			}

			if (ImGui::BeginDragDropSource()) {
				ImGui::SetDragDropPayload("explorer_drag_n_drop", "", sizeof(const char*));
				_pwd_dragged = entry.path();
				ImGui::EndDragDropSource();
			}
		}
	}

	void FileExplorer::GetDraggedPath(std::filesystem::path& pwd) {
		pwd = _pwd_dragged;
	}

} // namespace ImGui