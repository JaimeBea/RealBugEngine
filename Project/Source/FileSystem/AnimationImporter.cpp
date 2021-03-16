#include "AnimationImporter.h"

#include "Application.h"
#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleResources.h"
#include "Resources/ResourceAnimation.h"

#include "assimp/scene.h"
#include "assimp/anim.h"
#include "Math/float4x4.h"
#include "physfs.h"

#include "Utils/Leaks.h"

ResourceAnimation* AnimationImporter::ImportAnimation(const aiAnimation* aiAnim, const aiScene* assimpScene) {
	float ticks = static_cast<float>(aiAnim->mTicksPerSecond);
	float duration = static_cast<float>(aiAnim->mDuration);
	float durationInSeconds = ticks > 0 ? (duration / ticks) : 0;

	ResourceAnimation* anim = App->resources->ObtainAnimation();
	anim->duration = durationInSeconds;
	anim->fileName =  std::string(ANIMATIONS_PATH) + aiAnim->mName.C_Str() + std::string(ANIMATION_EXTENSION);

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

	SaveAnimation(anim);

	return anim;
}

void AnimationImporter::LoadAnimation(ResourceAnimation* animation) {
	MSTimer timer;
	timer.Start();

	Buffer<char> buffer = App->files->Load(animation->fileName.c_str());
	char* cursor = buffer.Data();

	animation->duration = *((float*) cursor);
	cursor += sizeof(float);
	unsigned numKreyFrames = *((unsigned*) cursor);
	cursor += sizeof(cursor);
	unsigned numChannels = *((unsigned*) cursor);
	cursor += sizeof(cursor);

	animation->keyFrames.resize(numKreyFrames);

	for (unsigned i = 0; i < numKreyFrames; ++i) {
		ResourceAnimation::KeyFrameChannels keyFrame;

		for (unsigned j = 0; j < numChannels; ++j) {
			ResourceAnimation::Channel channel;

			unsigned sizeName = *((unsigned*) cursor);
			cursor += sizeof(unsigned);

			char* name = (char*) malloc(sizeof(char) * (sizeName + 1));
			memcpy_s(name, sizeof(char) * sizeName, cursor, sizeof(char) * sizeName);
			cursor += (FILENAME_MAX / 2) * sizeof(char);

			name[sizeName] = '\0';

			channel.tranlation.x = *((float*) cursor);
			cursor += sizeof(float);
			channel.tranlation.y = *((float*) cursor);
			cursor += sizeof(float);
			channel.tranlation.z = *((float*) cursor);
			cursor += sizeof(float);

			channel.rotation.x = *((float*) cursor);
			cursor += sizeof(float);
			channel.rotation.y = *((float*) cursor);
			cursor += sizeof(float);
			channel.rotation.z = *((float*) cursor);
			cursor += sizeof(float);
			channel.rotation.w = *((float*) cursor);
			cursor += sizeof(float);

			keyFrame.channels.emplace(name, channel);

			free(name);
		}

		animation->keyFrames[i] = keyFrame;
	}

	unsigned timeMs = timer.Stop();
	LOG("Animation loaded in %ums", timeMs);
}

void AnimationImporter::SaveAnimation(ResourceAnimation* animation) {
	MSTimer timer;
	timer.Start();

	unsigned headerSize = sizeof(unsigned) * 2 + sizeof(float);

	unsigned channels = (animation->keyFrames.size() > 0) ? animation->keyFrames[0].channels.size() : 0;

	unsigned sizeKeyFramesV = animation->keyFrames.size() * channels * sizeof(ResourceAnimation::Channel);
	unsigned sizeKeyFramesK = animation->keyFrames.size() * channels * FILENAME_MAX / 2 * sizeof(char);
	unsigned sizeName = animation->keyFrames.size() * channels * sizeof(unsigned);
	unsigned bufferSize = headerSize + sizeKeyFramesV + sizeKeyFramesK + sizeName;

	Buffer<char> buffer(bufferSize);
	char* cursor = buffer.Data();

	*((float*) cursor) = animation->duration;
	cursor += sizeof(float);

	*((unsigned*) cursor) = animation->keyFrames.size();
	cursor += sizeof(unsigned);

	*((unsigned*) cursor) = channels;
	cursor += sizeof(unsigned);

	for (unsigned i = 0; i < animation->keyFrames.size(); ++i) {
		ResourceAnimation::KeyFrameChannels keyFrame = animation->keyFrames[i];

		for (auto it = keyFrame.channels.begin(); it != keyFrame.channels.end(); ++it) {
			ResourceAnimation::Channel channel = it->second;

			*((unsigned*) cursor) = it->first.length();
			cursor += sizeof(unsigned);

			memcpy_s(cursor, it->first.size() * sizeof(char), it->first.data(), it->first.size() * sizeof(char));
			cursor += (FILENAME_MAX / 2) * sizeof(char);

			// Translation
			*((float*) cursor) = channel.tranlation.x;
			cursor += sizeof(float);
			*((float*) cursor) = channel.tranlation.y;
			cursor += sizeof(float);
			*((float*) cursor) = channel.tranlation.z;
			cursor += sizeof(float);

			// Rotation
			*((float*) cursor) = channel.rotation.x;
			cursor += sizeof(float);
			*((float*) cursor) = channel.rotation.y;
			cursor += sizeof(float);
			*((float*) cursor) = channel.rotation.z;
			cursor += sizeof(float);
			*((float*) cursor) = channel.rotation.w;
			cursor += sizeof(float);
		}
	}

	LOG("Saving animation to \"%s\".", animation->fileName.c_str());
	App->files->Save(animation->fileName.c_str(), buffer);

	unsigned timeMs = timer.Stop();
	LOG("Animation saved in %ums", timeMs);
}
