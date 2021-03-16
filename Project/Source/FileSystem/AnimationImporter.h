#pragma once

class ResourceAnimation;
struct aiAnimation;
struct aiScene;

namespace AnimationImporter {

	ResourceAnimation* ImportAnimation(const aiAnimation* aiAnim, const aiScene* assimpScene);
	void LoadAnimation(ResourceAnimation* animation);
	void SaveAnimation(ResourceAnimation* animation);

};
