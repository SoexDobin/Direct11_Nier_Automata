#pragma once
#include "Camera.h"

NS_BEGIN(Client)

class CLIENT_DLL CinematicCamera final : public Camera
{
	RTTR_ENABLE(Camera)
public:
	typedef struct tagCinematicCameraDesc : public CAMERA_DESC
	{
		Vector3 startPosition{};
		Vector3 targetPosition{};
		Vector3 startRotation{};    // 시작 시점 회전 (Euler)
		Vector3 targetRotation{};   // 도착 시점 회전 (Euler)
		
		Float startFov{ 60.f };     // 시작 FOV (Degree)
		Float targetFov{ 60.f };    // 도착 FOV (Degree)
		
		Float startWaitTime{ 0.f };
		Float travelTime{ 2.f };    // 시작 지점에서 도착 지점까지 이동하는 시간
		Float returnTime{ 1.f };    // 도착 후 메인 카메라로 복귀하는 전환 시간
	} CINEMATIC_CAMERA_DESC;

public:
	explicit CinematicCamera() = default;
	explicit CinematicCamera(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit CinematicCamera(const CinematicCamera& rhs);
	virtual ~CinematicCamera() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;

public:
	void InvokeCinematic(); 

private:
	CINEMATIC_CAMERA_DESC m_CineDesc{};
	Bool  m_IsInvoked{ false };
	Float m_AccumulatedTime{ 0.f };
	Shared<Camera> m_PrevCamera{ nullptr };

	Vector3 m_InitialRotation{};
	Float   m_InitialFov{};

public:
	static Shared<CinematicCamera> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END
