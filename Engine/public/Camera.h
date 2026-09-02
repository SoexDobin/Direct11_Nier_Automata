#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL Camera abstract : public GameObject
{
	RTTR_ENABLE(GameObject)
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
	void Set_FovY(Float fov) { m_FovY = fov; }
	Float Get_Aspect() const { return m_Aspect; }
	void Set_Aspect(Float aspect);
	Float Get_NearPlane() const { return m_Near; }
	void Set_NearPlane(Float nearPlane) { m_Near = nearPlane; }
	Float Get_FarPlane() const { return m_Far; }
	void Set_FarPlane(Float farPlane) { m_Far = farPlane; }

public:

	void Bind_Aspect(Float aspect);

public:
	void Set_Target(const Shared<GameObject>& target);
	Shared<GameObject> Get_Target() const;
	ObjectGuid Get_TargetObjectGuid() const { return m_TargetGuid; }
	HRESULT Set_TargetObjectGuid(ObjectGuid targetGuid);
	uint32 Get_TargetID() const { return m_TargetID; }
	void Set_TargetID(uint32 targetID);

public:
	GAMEOBJECTTYPE Get_GameObjectType() final { return GAMEOBJECTTYPE::CAMERA; }
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
	Weak<GameObject> m_Target{};
	ObjectGuid	m_TargetGuid{};
	uint32		m_TargetID = 0;

	Float		m_FovY{}, m_Aspect{}, m_Near{}, m_Far{};

public:
	virtual Shared<GameObject> Clone(void* arg) PURE;
};

NS_END
