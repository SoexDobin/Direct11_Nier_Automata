#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ContainerObject;

class ENGINE_DLL PartObject abstract : public GameObject
{
	RTTR_ENABLE(GameObject)
public:
	typedef struct tagPartObjectDesc : public GameObject::GAMEOBJECT_DESC {
		Shared<ContainerObject> Owner{nullptr};
		Matrix* parentMatrix;
	} PARTOBJECT_DESC;

public:
	explicit PartObject();
	explicit PartObject(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit PartObject(const PartObject& rhs);
	virtual ~PartObject() override = default;
	
public:
	Shared<ContainerObject> Get_Owner() const { return m_Owner.lock(); }

public:
	GAMEOBJECTTYPE Get_GameObjectType() final { return GAMEOBJECTTYPE::PART; }
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override { GameObject::On_Destroy(); }
	void On_Enable() override { GameObject::On_Enable(); }
	void On_Disable() override { GameObject::On_Disable(); }

public:
	void Priority_Update(Float timeDelta) override {}
	void Update(Float timeDelta) override {}
	void Late_Update(Float timeDelta) override {}
	void Fixed_Update(Float fixedDelta) override {}
	HRESULT Render() override { return S_OK; }
	virtual void Submit_RenderGroup() override {}

public:
	Matrix* Get_CombinedWorldMatrix() { return &m_CombinedWorldMatrix; }
	void Update_CombineWorldMatrix(const Matrix& childMatrix) {
		m_CombinedWorldMatrix = childMatrix * *m_ParentMatrix;
	}

protected:
	Weak<ContainerObject> m_Owner{};
	Matrix*			m_ParentMatrix{ nullptr };
	Matrix			m_CombinedWorldMatrix{};

public:
	virtual Shared<GameObject> Clone(void* arg) PURE;
};

NS_END