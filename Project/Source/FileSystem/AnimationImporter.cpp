#include "AnimationImporter.h"

#include "Application.h"
#include "Modules/ModuleResources.h"
#include "Resources/ResourceAnimation.h"

#include "assimp/scene.h"
#include "assimp/anim.h"
#include "Math/float4x4.h"

#include "Utils/Leaks.h"

ResourceAnimation* AnimationImporter::ImportAnimation(const aiAnimation* aiAnim, const aiScene* assimpScene) {
	float ticks = static_cast<float>(aiAnim->mTicksPerSecond);
	float duration = static_cast<float>(aiAnim->mDuration);
	float durationInSeconds = ticks > 0 ? (duration / ticks) : 0;

	ResourceAnimation* anim = App->resources->ObtainAnimation();
	anim->duration = durationInSeconds;
	anim->name = aiAnim->mName.C_Str();

	unsigned int size = std::max(aiAnim->mChannels[0]->mNumPositionKeys, aiAnim->mChannels[0]->mNumRotationKeys);

	anim->keyFrames.resize(size);

	aiNode* rootNode = assimpScene->mRootNode;

	for (unsigned int i = 0; i < aiAnim->mNumChannels; ++i) {
		aiNodeAnim* aiChannel = aiAnim->mChannels[i];
		std::string channelName(aiChannel->mNodeName.C_Str());

		aiNode* parent = rootNode->FindNode(aiChannel->mNodeName.C_Str())->mParent;

		size_t pos = channelName.find("_$AssimpFbx$");
		if (pos != std::string::npos) {
			channelName = channelName.substr(0, pos);
		}

		float4x4 accumulatedTransform = float4x4::identity;
		bool assimpNode = (std::string(parent->mName.C_Str()).find("$AssimpFbx$") != std::string::npos);

		while (assimpNode) {
			aiVector3D pos, scale;
			aiQuaternion rot;
			(parent->mTransformation).Decompose(scale, rot, pos);
			accumulatedTransform = float4x4::FromTRS(float3(pos.x, pos.y, pos.z), Quat(rot.x, rot.y, rot.z, rot.w), float3(scale.x, scale.y, scale.z)) * accumulatedTransform;
			parent = parent->mParent;
			assimpNode = (std::string(parent->mName.C_Str()).find("$AssimpFbx$") != std::string::npos);
		}

		unsigned int frame = 0;

		for (unsigned int j = 0; j < size; ++j) {
			aiQuaternion aiQuat = (aiChannel->mNumRotationKeys > 1) ? aiChannel->mRotationKeys[j].mValue : aiChannel->mRotationKeys[0].mValue;
			aiVector3D aiV3D = (aiChannel->mNumPositionKeys > 1) ? aiChannel->mPositionKeys[j].mValue : aiChannel->mPositionKeys[0].mValue;

			float3 accumulatedPosition, accumulatedScaling;
			Quat accumulatedRotation;
			accumulatedTransform.Decompose(accumulatedPosition, accumulatedRotation, accumulatedScaling);

			ResourceAnimation::Channel channel = anim->keyFrames[frame].channels[channelName];
			channel.rotation = channel.rotation * accumulatedRotation * Quat(aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w);
			channel.tranlation = channel.tranlation + accumulatedPosition + float3(aiV3D.x, aiV3D.y, aiV3D.z);

			anim->keyFrames[frame++].channels[channelName] = channel;
		}
	}
	return anim;
}

void AnimationImporter::UnloadAnimation(ResourceAnimation* animation) {
	animation->keyFrames.clear();
}
