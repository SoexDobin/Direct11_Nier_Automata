#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL Camera abstract : public GameObject
{
public:
	typedef struct tagCameraDesc {
		Vector3 eye{}, at{}, up{ Vector3::Up };
		Float fovY = {};
		Float aspect = {};
		Float nearPlane = {};
		Float farPlane = {};
	} CAMERA_DESC;

public:
	Camera();
	Camera(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Camera(const Shared<Camera>& rhs);
	virtual ~Camera() override = default;

public:
	virtual void Set_Active(Bool isActive) override { GameObject::Set_Active(isActive); }
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* arg) override;
	virtual void On_Destroy() override { GameObject::On_Destroy(); }
	virtual void On_Enable() override { GameObject::On_Enable(); }
	virtual void On_Disable() override { GameObject::On_Disable(); }
	virtual void Priority_Update(Float timeDelta) override { GameObject::Priority_Update(timeDelta); }
	virtual void Update(Float timeDelta) override { GameObject::Update(timeDelta); }
	virtual void Late_Update(Float timeDelta) override { GameObject::Late_Update(timeDelta); }
	virtual void Fixed_Update(Float fixedDelta) override { GameObject::Fixed_Update(fixedDelta); }
	virtual HRESULT Render() override { return GameObject::Render(); }
	virtual Shared<GameObject> Clone(void* arg) PURE;
	
protected:
	void Update_TransformMatrices();

protected:
	Float		m_FovY{}, m_Aspect{}, m_Near{}, m_Far{};

};

NS_END