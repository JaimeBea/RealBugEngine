#include "AnimationController.h"

#include "Resources/ResourceAnimation.h"
#include "Resources/ResourceStateMachine.h"
#include "Resources/AnimationInterpolation.h"
#include "Resources/ResourceStates.h"
#include "Resources/Clip.h"
#include "GameObject.h"
#include "Math/float3.h"
#include "Modules/ModuleTime.h"
#include "Application.h"

#include "Utils/Leaks.h"

AnimationController::AnimationController(/*ResourceAnimation* resourceAnimation*/)
	/*: animationResource(resourceAnimation)*/ {
	running = true;
}

bool AnimationController::GetTransform(Clip* clip, float& currentTime, const char* name, float3& pos, Quat& quat) {
	if (clip->animationUID == 0) {
		return false;
	}

	if (clip->loop) {
		currentTime = currentTime > clip->duration ? 0 : currentTime;
	} else {
		currentTime = currentTime > clip->duration ? clip->duration : currentTime;
	}

	float currentSample = (currentTime * (clip->keyFramesSize - 1)) / clip->duration;
	currentSample += clip->beginIndex;
	int intPart = (int) currentSample;
	float decimal = currentSample - intPart;

	//find in hash by name
	ResourceAnimation* resourceAnimation = clip->GetResourceAnimation();
	std::unordered_map<std::string, ResourceAnimation::Channel>::const_iterator channel = resourceAnimation->keyFrames[intPart].channels.find(name);
	unsigned int idNext = intPart == (clip->keyFramesSize - 1) ? 0 : intPart + 1;
	std::unordered_map<std::string, ResourceAnimation::Channel>::const_iterator channelNext = resourceAnimation->keyFrames[idNext].channels.find(name);

	if (channel == resourceAnimation->keyFrames[intPart].channels.end() && channelNext == resourceAnimation->keyFrames[idNext].channels.end()) {
		return false;
	}

	pos = float3::Lerp(channel->second.tranlation, channelNext->second.tranlation, decimal);
	quat = Interpolate(channel->second.rotation, channelNext->second.rotation, decimal);

	return true;
}

bool AnimationController::InterpolateTransitions(std::list<AnimationInterpolation*>::iterator it, std::list<AnimationInterpolation*> animationInterpolations, GameObject* gameObject, float3& pos, Quat& quat) {
	bool ok = GetTransform((*it)->state->clip, (*it)->currentTime, gameObject->name.c_str(), pos, quat);
	if ((*it) != (*std::prev(animationInterpolations.end()))) {
		float3 position;
		Quat rotation;
		InterpolateTransitions(std::next(it), animationInterpolations, gameObject, position, rotation);

		(*it)->fadeTime = 1 - ((*it)->TransistionTime - (*it)->currentTime) / (*it)->TransistionTime;
		pos = float3::Lerp(position, pos, (*it)->fadeTime);
		quat = Interpolate(rotation, quat, (*it)->fadeTime);
	}
	if (gameObject->name.compare("Root") == 0) {
		(*it)->currentTime += App->time->GetDeltaTime();
	}

	return ok;
}

Quat AnimationController::Interpolate(const Quat& first, const Quat& second, float lambda) const {
	if (first.Dot(second) >= 0.0f) // is minimum arc ?
		return Quat::Lerp(first, second, lambda).Normalized();
	else
		return Quat::Lerp(first, second.Neg(), lambda).Normalized();
}