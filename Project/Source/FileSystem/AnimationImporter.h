#pragma once

class ResourceAnimation;
struct aiAnimation;
struct aiScene;

namespace AnimationImporter {

	ResourceAnimation* ImportAnimation(const aiAnimation* aiAnim, const aiScene* assimpScene);
	void UnloadAnimation(ResourceAnimation* animation);

};
