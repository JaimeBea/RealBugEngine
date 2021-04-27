#pragma once
#include "Utils/PoolMap.h"
#include "ParticleSystem/Particle.h";
#include "ParticleSystem/ParticleModule.h";
#include "Utils/UID.h"
#include "Component.h"
#include "Math/float4.h"

#include <vector>

class ComponentTransform;
class ParticleModule;

enum class EmitterType {
	CONE,
	SPHERE,
	HEMISPHERE,
	DONUT,
	CIRCLE,
	RECTANGLE
};

class ComponentParticleSystem : public Component {
public:
	REGISTER_COMPONENT(ComponentParticleSystem, ComponentType::PARTICLE, false);

	void Update() override;
	void DrawGizmos() override;
	float3 CreatePosition();
	void OnEditorUpdate() override;
	void Load(JsonValue jComponent) override;
	void Save(JsonValue jComponent) const override;
	void Draw();
	void DuplicateComponent(GameObject& owner) override;

private:
	const float4& GetTintColor() const; // Gets an additional color that needs to be applied to the image. Currently gets the color of the Button
	void CreateParticles(float nParticles, float vel);

private:
	float4 color = float4::one;		// Color used as default tainter
	bool alphaTransparency = false; // Enables Alpha Transparency of the image and the color
	UID textureID = 0;				// ID of the image
	UID shaderID = 0;				// ID of the shader

	int maxDistance = 2;

	float duration;
	bool looping;
	float startLifetime;

	float maxParticles = 100;
	float velocity = 0;

	float kc = 1.0f; //Keep in one to avoid having denominator less than 1
	float kl = 0.045f;
	float kq = 0.0075f;
	float innerAngle = pi / 12;
	float outerAngle = pi / 6;

	float4 initC;
	float4 finalC;

	float currentFrame;
	float3 translations[100];
	int aux = 0;

private:
	EmitterType emitterType = EmitterType::CONE;

	float3 pos = {0.0f, 0.0f, 0.0f};	   // World coordinates from where the light is emitting. Corresponds to the GameObject Transform and will update on OnTransformUpdate().
	float3 direction = {0.0f, 0.0f, 0.0f}; // Direction the light emits to. Corresponds to the GameObject Rotation and will update on OnTransformUpdate().

	std::vector<ParticleModule*> particlesModule;
	//TODO Particles should be a pool not a vector
	std::vector<Particle> particles;
};
