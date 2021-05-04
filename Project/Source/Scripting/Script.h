#pragma once

#include "Utils/UID.h"
#include "Creator.h"
#include "Member.h"

#include <vector>

#define GENERATE_BODY(classname)                            \
public:                                                     \
	classname(GameObject* owner_) : Script(owner_) {}       \
	const std::vector<Member>& GetMembers() const override; \
                                                            \
private:                                                    \
	static const CreatorImplementation<classname> creator;

#define GENERATE_BODY_IMPL(classname)                                      \
	const CreatorImplementation<classname> classname::creator(#classname); \
                                                                           \
	const std::vector<Member>& classname::GetMembers() const {             \
		return _members;                                                   \
	}

class Script {
public:
	Script(GameObject* owner_)
		: owner(owner_) {}

	virtual ~Script() = default;

	virtual const std::vector<Member>& GetMembers() const = 0;

	GameObject& GetOwner() const {
		return *owner;
	}

	virtual void Update() = 0;
	virtual void Start() = 0;
	virtual void OnButtonClick() {}

private:
	GameObject* owner = nullptr;
};