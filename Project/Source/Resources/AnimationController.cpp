#include "AnimationController.h"
#include "ResourceAnimation.h"
#include "ResourceStateMachine.h"
#include "Math/float3.h"
#include "Modules/ModuleTime.h"
#include "Application.h"

#include "Utils/Leaks.h"

AnimationController::AnimationController(ResourceAnimation* resourceAnimation)
	: animationResource(resourceAnimation) {
	currentTime = 0;
	loop = true;
	running = true;
}

bool AnimationController::GetTransform(const char* name, float3& pos, Quat& quat) {
	if (animationResource == nullptr) {
		return false;
	}	

	float currentSample = (currentTime * (animationResource->keyFrames.size() - 1)) / animationResource->duration;
	int intPart = (int) currentSample;
	float decimal = currentSample - intPart;

	//find in hash by name
	std::unordered_map<std::string, ResourceAnimation::Channel>::const_iterator channel = animationResource->keyFrames[intPart].channels.find(name);
	unsigned int idNext = intPart == (animationResource->keyFrames.size() - 1) ? 0 : intPart + 1;
	std::unordered_map<std::string, ResourceAnimation::Channel>::const_iterator channelNext = animationResource->keyFrames[idNext].channels.find(name);	

	if (channel == animationResource->keyFrames[intPart].channels.end() && channelNext == animationResource->keyFrames[idNext].channels.end()) {
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
	if (loop) {
		currentTime = currentTime > animationResource->duration ? 0 : currentTime;
	} else {
		currentTime = currentTime > animationResource->duration ? animationResource->duration : currentTime;
	}
}

Quat AnimationController::Interpolate(const Quat& first, const Quat& second, float lambda) const {
	if (first.Dot(second) >= 0.0f) // is minimum arc ?
		return Quat::Lerp(first, second, lambda).Normalized();
	else
		return Quat::Lerp(first, second.Neg(), lambda).Normalized();
}