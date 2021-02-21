#include "AnimationController.h"
#include "Math/float3.h"
bool AnimationController::GetTransform(const char* name, float3& pos, Quat& quat) {
	/*if (animationResource == nullptr) {
		return 0;
	}*/

	//find in hash by name

	//channelFound = animationResource->find(name);
	//if (!channelFound){
	//	return 0;
	//}

/*	if (loop) {
		currentTime = currentTime % animationResource->GetDuration();
	} else {
		currentTime = currentTime > animationResource->GetDuration() ? animationResource->GetDuration() : currentTime;
	}

	float currentSample = currentTime * (animationResource->GetNumSamples() - 1) / animationResource->GetDuration() ;
	int intPart = (int) currentSample;
	float decimal = currentSample - intPart;

	if (decimal <= 0.001f) {
		pos = channelFound->GetPosition(intPart);
		quat = channelFound->GetRotation(intPart);
		return;
	} 

	float lambda = 1 - decimal;
	pos = float3::Lerp(channelFound->GetPosition(intPart), channelFound->GetPosition(intPart + 1), lambda);
	quat = Interpolate(channelFound->GetRotation(intPart), channelFound->GetRotation(intPart + 1), lambda);*/
}

void AnimationController::Play() {
	running = true;
}

void AnimationController::Stop() {
	running = false;
}

void AnimationController::Update() {
	//currentTime = elapsedTime; 
}

Quat AnimationController::Interpolate(const Quat& first, const Quat& second, float lambda) const {
	if (first.Dot(second) >= 0.0f) // is minimum arc ?
		return Quat::Lerp(first, second, lambda).Normalized();
	else
		return Quat::Lerp(first, second.Neg(), lambda).Normalized();
}