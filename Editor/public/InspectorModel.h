#pragma once
#include "EditorObject.h"
#include <filesystem>

NS_BEGIN(Engine)
class Component;
NS_END

NS_BEGIN(Editor)

class InspectorModel : public EditorObject
{
public:
	InspectorModel() = default;
	~InspectorModel() override = default;

public:
	HRESULT Initialize();
	void Render(Bool isResize) override {} 
	void RenderComponent(const std::shared_ptr<Engine::Component>& pComp);

public:
	static std::shared_ptr<InspectorModel> Create();

private:
	std::filesystem::path m_SelectedPresetPath;
	string m_PresetStatus;
};

NS_END
