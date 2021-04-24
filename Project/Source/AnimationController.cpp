#include "AnimationController.h"

#include "Application.h"
#include "GameObject.h"
#include "States.h"
#include "Resources/ResourceAnimation.h"
#include "AnimationInterpolation.h"
#include "Resources/ResourceClip.h"
#include "Math/float3.h"
#include "Modules/ModuleTime.h"

#include "Utils/Leaks.h"

bool AnimationController::GetTransform(const ResourceClip& clip, float& currentTime, const char* name,float3& pos,Quat& quat) {
	assert(clip.animationUID != 0);
	
	if (clip.loop) {
		currentTime = currentTime > clip.duration ? 0 : currentTime;
	} else {
		currentTime = currentTime > clip.duration ? clip.duration : currentTime;
	}

	float currentSample = (currentTime * (clip.keyFramesSize - 1)) / clip.duration;
	currentSample += clip.beginIndex;
	int intPart = (int) currentSample;
	float decimal = currentSample - intPart;

	//find in hash by name
	ResourceAnimation* resourceAnimation = clip.GetResourceAnimation();
	std::unordered_map<std::string, ResourceAnimation::Channel>::const_iterator channel = resourceAnimation->keyFrames[intPart].channels.find(name);
	unsigned int idNext = intPart == (clip.keyFramesSize - 1) ? 0 : intPart + 1;
	std::unordered_map<std::string, ResourceAnimation::Channel>::const_iterator channelNext = resourceAnimation->keyFrames[idNext].channels.find(name);

	if (channel == resourceAnimation->keyFrames[intPart].channels.end() && channelNext == resourceAnimation->keyFrames[idNext].channels.end()) {
		return false;
	}

	pos = float3::Lerp(channel->second.tranlation, channelNext->second.tranlation, decimal);
	quat = AnimationController::Interpolate(channel->second.rotation, channelNext->second.rotation, decimal);

	return true;
}

bool AnimationController::InterpolateTransitions(const std::list<AnimationInterpolation>::iterator &it, std::list<AnimationInterpolation> &animationInterpolations, GameObject& rootBone, GameObject& gameObject, float3& pos, Quat& quat) {
	ResourceClip clip = *(App->resources->GetResource<ResourceClip>((*it).state->clipUid));
	bool ok = GetTransform(clip, (*it).currentTime, gameObject.name.c_str(), pos, quat);
	if (&(*it) != &(*std::prev(animationInterpolations.end()))) {
		float3 position;
		Quat rotation;
		AnimationController::InterpolateTransitions(std::next(it), animationInterpolations, rootBone, gameObject, position, rotation);

		(*it).fadeTime = 1 - ((*it).TransistionTime - (*it).currentTime) / (*it).TransistionTime;
		pos = float3::Lerp(position, pos, (*it).fadeTime);
		quat = AnimationController::Interpolate(rotation, quat, (*it).fadeTime);
	}
	if (&gameObject == &rootBone) { // Only udate currentTime for the rootBone
		(*it).currentTime += App->time->GetDeltaTime();
	}

	return ok;
}

Quat AnimationController::Interpolate(const Quat& first, const Quat& second, float lambda) {
	if (first.Dot(second) >= 0.0f) // is minimum arc ?
		return Quat::Lerp(first, second, lambda).Normalized();
	else
		return Quat::Lerp(first, second.Neg(), lambda).Normalized();
}