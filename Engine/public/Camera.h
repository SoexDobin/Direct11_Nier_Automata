#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL Camera abstract : public GameObject
{
public:
	typedef struct tagCameraDesc {
		Vector4 eye{}, at{}, up{ Vector4{0.f,0.f,0.f,1.f} };
		Float fovY = {};
		Float aspect = {};
		Float nearPlane = {};
		Float farPlane = {};
	} CAMERA_DESC;

public:
	Camera();
	Camera(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Camera(const Camera& rhs);
	virtual ~Camera() override = default;

public:
	Float Get_FovY() const { return m_FovY; }
	Float Get_Aspect() const { return m_Aspect; }
	Float Get_NearPlane() const { return m_Near; }
	Float Get_FarPlane() const { return m_Far; }

public:
	void Set_Aspect(Float aspect) { m_Aspect = aspect; Bind_CameraTransform(); }

public:
	void Set_Active(Bool isActive) final { GameObject::Set_Active(isActive); }
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* arg) override;
	virtual void On_Destroy() override { GameObject::On_Destroy(); }
	virtual void On_Enable() override { GameObject::On_Enable(); }
	virtual void On_Disable() override { GameObject::On_Disable(); }

public:
	virtual void Priority_Update(Float timeDelta) override { GameObject::Priority_Update(timeDelta); }
	virtual void Update(Float timeDelta) override { GameObject::Update(timeDelta); }
	virtual void Late_Update(Float timeDelta) override { GameObject::Late_Update(timeDelta); }
	virtual void Fixed_Update(Float fixedDelta) override { GameObject::Fixed_Update(fixedDelta); }
	virtual HRESULT Render() override { return GameObject::Render(); }
	virtual Shared<GameObject> Clone(void* arg) PURE;
	
public:
	void Update_CameraTransform() const;
	void Bind_CameraTransform() const;

protected:
	Float		m_FovY{}, m_Aspect{}, m_Near{}, m_Far{};

};

NS_END