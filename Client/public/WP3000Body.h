#pragma once
#include "Pl0000Parts.h"

NS_BEGIN(Client)

class Pl0000Body;

class CLIENT_DLL WP3000Body final : public Pl0000Parts
{
	RTTR_ENABLE(Pl0000Parts)
public:
	enum class POD_STATE
	{
		IDLE			= 0,
		SHOOT_START		= 11,
		SHOOT_LOOP		= 12,
		SHOOT_END		= 13,
	};

public:
	explicit WP3000Body();
	explicit WP3000Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit WP3000Body(const WP3000Body& rhs);
	~WP3000Body() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;

public:
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void Fixed_Update(Float fixedDelta) override;
	HRESULT Render() override;
	void Submit_RenderGroup() override;

public:
	void Set_Pl0000Body(const Shared<Pl0000Body>& body) { m_Pl0000Body = body; }

private:
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Components();

private:
	void Pod_Fire(Float timeDelta);

private:
	Float m_FireRateTimer = 0.f;
	Float m_FireRate = 0.075f;

private:
	int32 m_RootBoneIndex = -1;
	Float m_HoverTime = 0.f;
	Float m_HoverAmplitude = 0.2f;   // 상하 진폭
	Float m_HoverSpeed = 2.0f;       // 상하 이동 속도
	Float m_FollowSpeed = 5.0f;      // 플레이어를 따라가는 보간 속도
	Vector3 m_TargetOffset = Vector3(0.8f, 1.5f, 0.f); // 옵셋 (우측, 위, 뒤)
	POD_STATE m_PodState = POD_STATE::IDLE;

	Weak<Pl0000Body> m_Pl0000Body{};

public:
	static Shared<WP3000Body> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END