#pragma once
#include "EditorObject.h"

NS_BEGIN(Engine)
class GameObject;
class Component;
NS_END

NS_BEGIN(Editor)

class InspectorTransform;
class InspectorModel;
class InspectorTexture;
class InspectorCamera;

class Inspector : public EditorObject
{
public:
	Inspector();
	~Inspector() override;

public:
	HRESULT Initialize() override;
	void Render(Bool isResize) override;

private:
	void GameObjectGUI(const Shared<Engine::GameObject>& obj);
	void Draw_GameObjectHeader(const Shared<Engine::GameObject>& obj);

private:
    Shared<InspectorTransform> m_InspectorTransform{ nullptr };
    Shared<InspectorModel> m_InspectorModel{ nullptr };
    Shared<InspectorTexture> m_InspectorTexture{ nullptr };
    Shared<InspectorCamera> m_InspectorCamera{ nullptr };
    uint32 m_iPrevSelectedID{ 0 };

public:
	static Shared<Inspector> Create();
};

NS_END