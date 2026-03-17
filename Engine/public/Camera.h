#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL Camera abstract : public GameObject
{
public:
	typedef struct tagCameraDesc : public GAMEOBJECT_DESC {
		Vector4 eye{}, at{}, up{ Vector4{0.f,0.f,0.f,1.f} };
		Float fovY = {};
		Float aspect = {};
		Float nearPlane = {};
		Float farPlane = {};
	} CAMERA_DESC;

public:
	explicit Camera();
	explicit Camera(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Camera(const Camera& rhs);
	virtual ~Camera() override = default;

public:
	Float Get_FovY() const { return m_FovY; }
	Float Get_Aspect() const { return m_Aspect; }
	Float Get_NearPlane() const { return m_Near; }
	Float Get_FarPlane() const { return m_Far; }

public:
	void Set_Aspect(Float aspect);
	void Bind_Aspect(Float aspect);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* arg) override;
	virtual void On_Destroy() override { GameObject::On_Destroy(); }
	virtual void On_Enable() override { GameObject::On_Enable(); }
	virtual void On_Disable() override { GameObject::On_Disable(); }

public:
	virtual void Priority_Update(Float timeDelta) override;
	virtual void Update(Float timeDelta) override { GameObject::Update(timeDelta); }
	virtual void Late_Update(Float timeDelta) override { GameObject::Late_Update(timeDelta); }
	virtual void Fixed_Update(Float fixedDelta) override { GameObject::Fixed_Update(fixedDelta); }
	virtual HRESULT Render() override { return GameObject::Render(); }
	
public:
	void Update_CameraTransform(Float timeDelta);
	void Bind_CameraTransform() const;


protected:
	Float		m_FovY{}, m_Aspect{}, m_Near{}, m_Far{};

public:
	virtual Shared<GameObject> Clone(void* arg) PURE;
};

NS_END