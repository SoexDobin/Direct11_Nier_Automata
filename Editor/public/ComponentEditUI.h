#pragma once
#include "EditorObject.h"
#include <rttr/property.h>
#include <rttr/type.h>
#include <functional>

NS_BEGIN(Engine)
class Component;
NS_END

NS_BEGIN(Editor)

class ComponentEditUI final : public EditorObject
{
public:
	ComponentEditUI();
	~ComponentEditUI() override = default;

public:
	HRESULT Initialize() override;
	void Update() override;
	void Render() override;

private:
	vector<string> memberNames;

public:
	static Shared<ComponentEditUI> Create();
};

NS_END