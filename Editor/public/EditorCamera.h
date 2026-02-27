#pragma once
#include "Camera.h"

NS_BEGIN(Editor)

class EditorCamera final : public Camera
{
public:
	EditorCamera();
	EditorCamera(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	EditorCamera(const Shared<EditorCamera>& rhs);
	~EditorCamera() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;

	void Set_Active(Bool isActive) override { Camera::Set_Active(isActive); }
	void On_Destroy() override { Camera::On_Destroy(); }
	void On_Enable() override { Camera::On_Enable(); }
	void On_Disable() override { Camera::On_Disable(); }

	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void Fixed_Update(Float fixedDelta) override;
	HRESULT Render() override { return S_OK; }
	
private:

public:
	static Shared<EditorCamera> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, void* cameraDesc);
	Shared<GameObject> Clone(void* arg) override { return nullptr; }
};

NS_END