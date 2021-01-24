#pragma once

class Texture;
class CubeMap;

namespace TextureImporter {
	Texture* ImportTexture(const char* file_path);
	void LoadTexture(Texture* texture);
	void UnloadTexture(Texture* texture);

	CubeMap* ImportCubeMap(const char* file_paths[6]);
	void LoadCubeMap(CubeMap* cube_map);
	void UnloadCubeMap(CubeMap* cube_map);
}; // namespace TextureImporter
