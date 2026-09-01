#pragma once
#include "EditorObject.h"

NS_BEGIN(Engine)
class Object;
class GameObject;
class Component;
NS_END

NS_BEGIN(Editor)

class InspectorTransform;
class InspectorModel;
class InspectorTexture;
class InspectorCamera;
class InspectorCollider;

class Inspector : public EditorObject
{
public:
	Inspector();
	~Inspector() override;

public:
	HRESULT Initialize() override;
	void Render(Bool isResize) override;

public:
	void Render_ProjectSettingsWindow();

private:
	void GameObjectGUI(const Shared<Engine::GameObject>& obj);
	void Draw_GameObjectHeader(const Shared<Engine::GameObject>& obj);
	void RenderGenericProperties(Engine::Object& object);

private:
    Shared<InspectorTransform> m_InspectorTransform{ nullptr };
    Shared<InspectorModel> m_InspectorModel{ nullptr };
    Shared<InspectorTexture> m_InspectorTexture{ nullptr };
    Shared<InspectorCamera> m_InspectorCamera{ nullptr };
	Shared<InspectorCollider> m_InspectorCollider{ nullptr };
    uint32 m_iPrevSelectedID{ 0 };

private:
	char m_LayerNames[32][64] = { 0 };
	char m_TagNames[32][64] = { 0 };
	Bool m_IsFirstLoadSettings = { true };

public:
	static Shared<Inspector> Create();
};

NS_END
