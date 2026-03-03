#pragma once
#include "EngineManager.h"

NS_BEGIN(Engine)

class Camera;

class CameraManager final : public EngineManager
{
	NO_COPY(CameraManager)
public:
	CameraManager();
	~CameraManager() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg = nullptr) override;
	
	void Set_Active(Bool isActive) override { EngineManager::Set_Active(isActive); }
	void On_Destroy() override { EngineManager::On_Destroy(); }
	void On_Disable() override { EngineManager::On_Disable(); }
	void On_Enable() override { EngineManager::On_Enable(); }
	
public:
	HRESULT Add_Camera(const Shared<Camera>& camera);
	Shared<Camera> Get_MainCamera() const;
	HRESULT Set_MainCamera(const Shared<Camera>& camera);
	void Bind_MainCamera_Transform();
	HRESULT Clear_Cameras();

private:
	vector<Weak<Camera>> m_Cameras;
	Weak<Camera> m_MainCamera{};

public:
	static Unique<CameraManager> Create();
	
};

NS_END