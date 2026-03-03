#pragma once
#include "Camera.h"

NS_BEGIN(Editor)

class EditorCamera final : public Camera
{
	RTTR_ENABLE(Camera)
public:
	typedef struct tagEditorCameraDesc final : public CAMERA_DESC
	{
		Float cameraSpeed{ 10.0f };
		Float mouseSensitive{ 0.1f };
	} EDITOR_CAMERA_DESC;

public:
	explicit EditorCamera(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	~EditorCamera() override = default;

public:
	HRESULT Initialize(void* arg) override;

	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void Fixed_Update(Float fixedDelta) override;
	HRESULT Render() override;

public:
	HRESULT Bind_EditorMatrix() const;
	Vector3 RaycastEditorView(Vector2 mousePos, Vector2 viewSize, _Out_ Shared<class GameOject>* outObjectAddress);
	
private:
	Float m_CameraSpeed{};
	Float m_MouseSensitive{};

public:
	static Shared<EditorCamera> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, EDITOR_CAMERA_DESC& desc);
	Shared<GameObject> Clone(void* arg) override { return nullptr; }
};

NS_END