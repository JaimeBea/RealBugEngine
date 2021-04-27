#pragma once
#include "Utils/UID.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"

class ComponentTransform;

class Particle {
public:
	Particle(UID idShader, UID texture, bool alphaTrans, float vel, float4 inC, float4 finalC, float3 p);
	~Particle();

	void Draw(ComponentTransform* transform);
	void Die();
	void Uptade();
	void Spawn(float3 p);

public:
	float animationSpeed = 24.0;
	float currentFrame = 0;
	float4x4 localModelMatrix = float4x4::identity;
	//float4 color = float4::one;		// Color used as default tainter
	bool alphaTransparency = true; // Enables Alpha Transparency of the image and the color
	UID textureID;				   // ID of the image
	UID shaderID;				   // ID of the shader
	float3 position;
	Quat rotation = Quat::identity;
	float3 scale = float3(0.5f, 0.5f, 0.5f);

	float4 initColor;
	float4 finalColor;

	float velocity;
	float life;
	bool isActivate = false;
};
