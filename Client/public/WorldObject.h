#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class Shader;
class Model;
class Navigation;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL WorldObject abstract : public GameObject
{
	RTTR_ENABLE(GameObject)
protected:
	typedef struct tagWorldObjectDesc : public GAMEOBJECT_DESC
	{
		wstring vertexTag{};
		wstring modelTag{};
		wstring navTag{};
	} WORLD_OBJECT_DESC;

public:
	explicit WorldObject() = default;
	explicit WorldObject(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit WorldObject(const WorldObject& rhs);
	virtual ~WorldObject() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;
	void On_Enable() override;
	void On_Disable() override;

public:
	HRESULT Render() override;
	void Submit_RenderGroup() override;

protected:
	virtual HRESULT Ready_Components(const WORLD_OBJECT_DESC& desc);
	virtual HRESULT Bind_ShaderResources() const;

protected:
	Shared<Shader> m_Shader{ nullptr };
	Shared<Model> m_Model{ nullptr };
	Shared<Navigation> m_Navigation{ nullptr };

public:
	Shared<GameObject> Clone(void* arg) override PURE;
};

NS_END

