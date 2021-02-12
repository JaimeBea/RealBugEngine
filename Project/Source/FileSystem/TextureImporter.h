#pragma once

class Texture;
class CubeMap;

namespace TextureImporter {
	Texture* ImportTexture(const char* filePath);
	void LoadTexture(Texture* texture);
	void UnloadTexture(Texture* texture);

	CubeMap* ImportCubeMap(const char* filePaths[6]);
	void LoadCubeMap(CubeMap* cubeMap);
	void UnloadCubeMap(CubeMap* cubeMap);
}; // namespace TextureImporter
