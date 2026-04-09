#pragma once
#include "Movement.h"

NS_BEGIN(Engine)
class Transform;
NS_END

NS_BEGIN(Client)

class Em0010;

class CLIENT_DLL Em0010Movement final : public Movement
{
	RTTR_ENABLE(Movement)
public:
	typedef struct tagEm0010MovementDesc : public MOVEMENT_DESC
	{

	} EM0010_MOVEMENT_DESC;

public:
	typedef struct tagEm0010MovementData
	{
		Vector3 direction{ 0.f, 0.f, 0.f };
		Vector3 lookDirection{ 0.f, 0.f, 0.f };

		Bool isMove{ false };
		Bool isJump{ false };
		Bool isAttack{ false };

		Bool canRotation{ true };
		Bool useRootMotionDir{ true };		// 로컬 방향에 의한 tilt 수정
	} Em0010_MOVEMENT_DATA;

public:
	void Set_MovementData(const Em0010_MOVEMENT_DATA& data) { m_CurrentMoveData = data; }
	const Em0010_MOVEMENT_DATA& Get_MovementData() const { return m_CurrentMoveData; }

public:
	explicit Em0010Movement();
	explicit Em0010Movement(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Em0010Movement(const Em0010Movement& rhs);
	~Em0010Movement() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg = nullptr) override;
	HRESULT Begin() override;

public:
	void Update_Movement(Float timeDelta) override;

private:
	Em0010_MOVEMENT_DATA m_CurrentMoveData{};
	Vector3 m_LastGroundedRootPositionVelocity{ 0.f, 0.f, 0.f };

private:
	Weak<Em0010> m_OwnerContainer{};

public:
	static Shared<Em0010Movement> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<Component> Clone(void* arg = nullptr) override;
};

NS_END
