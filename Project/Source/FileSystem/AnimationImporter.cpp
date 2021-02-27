#include "AnimationImporter.h"

#include "Resources/ResourceAnimation.h"

#include "assimp/anim.h"

ResourceAnimation* AnimationImporter::ImportAnimation(const aiAnimation* aiAnim) {
	float ticks = static_cast<float>(aiAnim->mTicksPerSecond);
	float duration = static_cast<float>(aiAnim->mDuration);
	float durationInSeconds = ticks > 0 ? (duration / ticks) : 0;

	ResourceAnimation* anim = new ResourceAnimation();
	anim->duration = durationInSeconds;
	anim->name = aiAnim->mName.C_Str();

	unsigned int size = std::max(aiAnim->mChannels[0]->mNumPositionKeys, aiAnim->mChannels[0]->mNumRotationKeys);

	anim->keyFrames.resize(size);

	for (unsigned int i = 0; i < aiAnim->mNumChannels; ++i) {
		aiNodeAnim* aiChannel = aiAnim->mChannels[i];
		std::string channelName(aiChannel->mNodeName.C_Str());

		unsigned int type = 0; // 0 = normal, 1 = translation, 2 = rotation
		size_t pos = channelName.find("_$AssimpFbx$");
		if (pos != std::string::npos) {
			size_t tran = channelName.find("Translation");
			if (tran != std::string::npos) {
				type = 1;
			} else {
				type = 2;
			}
			channelName = channelName.substr(0, pos);
		}

		unsigned int frame = 0;

		for (unsigned int j = 0; j < size; ++j) {

			aiQuaternion aiQuat = (aiChannel->mNumRotationKeys > 1) ? aiChannel->mRotationKeys[j].mValue : aiChannel->mRotationKeys[0].mValue;
			aiVector3D aiV3D = (aiChannel->mNumPositionKeys > 1) ? aiChannel->mPositionKeys[j].mValue : aiChannel->mPositionKeys[0].mValue;

			ResourceAnimation::Channel channel = anim->keyFrames[frame].channels[channelName];
			if (type == 0 || type == 2) channel.rotation = channel.rotation * Quat(aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w);
			if (type == 0 || type == 1) channel.tranlation = channel.tranlation + float3(aiV3D.x, aiV3D.y, aiV3D.z);

			anim->keyFrames[frame++].channels[channelName] = channel;

		}
	}
	return anim;
}