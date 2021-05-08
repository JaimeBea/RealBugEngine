#include "EnemySpawnPoint.h"

#include "Resources/ResourcePrefab.h"
#include "GameplaySystems.h"
#include "GameObject.h"
#include <string>

EXPOSE_MEMBERS(EnemySpawnPoint) {
	MEMBER(MemberType::INT, amountOfEnemies),
		MEMBER(MemberType::PREFAB_RESOURCE_UID, prefabId)
};

GENERATE_BODY_IMPL(EnemySpawnPoint);

void EnemySpawnPoint::Start() {
	gameObject = &GetOwner();
}

void EnemySpawnPoint::Update() {
	for (; iterator < amountOfEnemies; ++iterator) {
		ResourcePrefab* prefab = GameplaySystems::GetResource<ResourcePrefab>(prefabId);
		if (prefab != nullptr) {
			UID prefabId = prefab->BuildPrefab(gameObject);
			GameObject* go = GameplaySystems::GetGameObject(prefabId);
			ComponentTransform* goTransform = go->GetComponent<ComponentTransform>();
			ComponentBoundingBox* goBounds = nullptr;
			for (auto& child : go->GetChildren()) {
				if (child->HasComponent<ComponentMeshRenderer>()) {
					goBounds = child->GetComponent<ComponentBoundingBox>();
					break;
				}
			}
			if (goTransform && goBounds) {
				float3 newPosition = goTransform->GetPosition();
				int val = (goBounds->GetLocalMaxPointAABB() - goBounds->GetLocalMinPointAABB()).x;
				newPosition.x += iterator * offset * val;
				Debug::Log(std::to_string(val).c_str());
				goTransform->SetPosition(newPosition);
			}
		}
	}
}