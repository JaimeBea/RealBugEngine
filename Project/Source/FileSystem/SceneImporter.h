#pragma once

class GameObject;

namespace SceneImporter {
	bool ImportScene(const char* file_path, GameObject* parent);
	bool LoadScene(const char* file_name);
	bool SaveScene(const char* file_name);
} // namespace SceneImporter
