#include "AnimationController.h"
#include "ResourceAnimation.h"
#include "ResourceStateMachine.h"
#include "AnimationInterpolation.h"
#include "ResourceStates.h"
#include "Clip.h"
#include "GameObject.h"
#include "Math/float3.h"
#include "Modules/ModuleTime.h"
#include "Application.h"

#include "Utils/Leaks.h"

AnimationController::AnimationController(ResourceAnimation* resourceAnimation)
	: animationResource(resourceAnimation) {
	currentTime = 0;
	running = true;
}

bool AnimationController::GetTransform(Clip* clip, const char* name, float3& pos, Quat& quat) {
	if (clip->animation == nullptr) {
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
	std::unordered_map<std::string, ResourceAnimation::Channel>::const_iterator channel = clip->animation->keyFrames[intPart].channels.find(name);
	unsigned int idNext = intPart == (clip->keyFramesSize - 1) ? 0 : intPart + 1;
	std::unordered_map<std::string, ResourceAnimation::Channel>::const_iterator channelNext = clip->animation->keyFrames[idNext].channels.find(name);	

	if (channel == clip->animation->keyFrames[intPart].channels.end() && channelNext == clip->animation->keyFrames[idNext].channels.end()) {
		return false;
	}

	float lambda = 1 - decimal;
	pos = float3::Lerp(channel->second.tranlation, channelNext->second.tranlation, lambda);
	quat = Interpolate(channel->second.rotation, channelNext->second.rotation, lambda);

	return true;
}

void AnimationController::Play() {
	running = true;
}

void AnimationController::Stop() {
	running = false;
}

void AnimationController::Update() {
	currentTime += App->time->GetDeltaTime();
}

bool AnimationController::InterpolateTransitions(std::list<AnimationInterpolation*>::iterator it, std::list<AnimationInterpolation*> animationInterpolations, GameObject* gameObject, float3& pos, Quat& quat) {
	bool ok = GetTransform((*it)->state->clip, gameObject->name.c_str(), pos, quat);
	if ((*it) != (*std::prev(animationInterpolations.end()))) {
		float3 position;
		Quat rotation;
		InterpolateTransitions(std::next(it), animationInterpolations, gameObject, position, rotation);

		(*it)->fadeTime = 1 - ((*it)->TransistionTime - (*it)->currentTime) / (*it)->TransistionTime;
		pos = float3::Lerp(pos, position, (*it)->fadeTime);
		quat = Interpolate(quat, rotation, (*it)->fadeTime);
	}
	(*it)->currentTime += App->time->GetDeltaTime();
	return ok;
}

Quat AnimationController::Interpolate(const Quat& first, const Quat& second, float lambda) const {
	if (first.Dot(second) >= 0.0f) // is minimum arc ?
		return Quat::Lerp(first, second, lambda).Normalized();
	else
		return Quat::Lerp(first, second.Neg(), lambda).Normalized();
}