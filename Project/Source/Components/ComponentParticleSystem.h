#pragma once
#include "Utils/Pool.h"
#include "Utils/UID.h"
#include "Component.h"
#include "Math/float4.h"
#include "Math/float2.h"
#include <vector>
#include "Math/float4x4.h"
#include "Math/Quat.h"
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
	struct Particle {
		float4x4 model;

		float3 initialPosition;
		float3 position;
		float3 direction;
		float3 scale;

		Quat rotation;

		float velocity;
		float life = 0.0F;
		float currentFrame;
		float colorFrame;
	};

	REGISTER_COMPONENT(ComponentParticleSystem, ComponentType::PARTICLE, false);

	void Update() override;
	void Init() override;
	void DrawGizmos() override;
	void OnEditorUpdate() override;
	void Load(JsonValue jComponent) override;
	void Save(JsonValue jComponent) const override;
	void Draw();
	void DuplicateComponent(GameObject& owner) override;
	void SpawnParticle();

	float3 CreatePosition();
	float3 CreateVelocity();

private:
	const float4& GetTintColor() const; // Gets an additional color that needs to be applied to the image. Currently gets the color of the Button
	void CreateParticles(float nParticles, float vel);

private:
	UID textureID = 0; // ID of the image
	UID shaderID = 0;  // ID of the shader

	bool looping;
	bool alphaTransparency = false; // Enables Alpha Transparency of the image and the color

	float4 initC;
	float4 finalC;
	float4 color = float4::one; // Color used as default tainter

	float scale = 0.1;
	float duration;
	float startLifetime;
	float maxParticles = 100;
	float velocity = 0.1;
	float kc = 1.0f; //Keep in one to avoid having denominator less than 1
	float kl = 0.045f;
	float kq = 0.0075f;
	float innerAngle = pi / 12;
	float outerAngle = pi / 6;

	int maxDistance = 2;
	int particleActivated;
	int Xtiles = 1;
	int Ytiles = 1;

private:
	EmitterType emitterType = EmitterType::CONE;

	std::vector<ParticleModule*> particlesModule;
	//TODO Particles should be a pool not a vector
	Pool<Particle> particles;
	std::vector<Particle*> deadParticles;
	float3 particlesPosition[100];
};
