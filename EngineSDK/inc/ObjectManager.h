#pragma once
#include "Manager.h"

NS_BEGIN(Engine)

class GameObject;

class ObjectManager final : public Manager
{
	NO_COPY(ObjectManager)
public:
	ObjectManager() = default;
	~ObjectManager() override = default;

private:
	unordered_map<uint32, list<Shared<GameObject>>> m_Objects;

public:
	static Unique<ObjectManager> Create();
};

NS_END