#include "AnimationController.h"
#include "ResourceAnimation.h"
#include "Math/float3.h"
#include "Modules/ModuleTime.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "Globals.h"

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

	if (loop) {
		currentTime = currentTime % (animationResource->keyFrames.size() - 1);
	} else {
		currentTime = currentTime > animationResource->duration ? animationResource->duration : currentTime;
	}

	float currentSample = currentTime; //( currentTime * (animationResource->keyFrames.size() - 1) ) / animationResource->duration;
	int intPart = (int) currentSample;
	float decimal = currentSample - intPart;

	//find in hash by name
	std::unordered_map<std::string, ResourceAnimation::Channel>::const_iterator channel = animationResource->keyFrames[intPart].channels.find(name);
	std::unordered_map<std::string, ResourceAnimation::Channel>::const_iterator channelNext = animationResource->keyFrames[intPart + 1].channels.find(name);

	if (channel == animationResource->keyFrames[intPart].channels.end() && channelNext == animationResource->keyFrames[intPart + 1].channels.end()) {
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
	//currentTime = currentTime + App->time->GetDeltaTime();
	currentTime = App->time->GetFrameCount();
	//currentTime = 0;
}

Quat AnimationController::Interpolate(const Quat& first, const Quat& second, float lambda) const {
	if (first.Dot(second) >= 0.0f) // is minimum arc ?
		return Quat::Lerp(first, second, lambda).Normalized();
	else
		return Quat::Lerp(first, second.Neg(), lambda).Normalized();
}