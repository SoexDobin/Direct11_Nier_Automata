#pragma once
#include "Movement.h"

NS_BEGIN(Engine)
class Transform;
class Navigation;
NS_END

NS_BEGIN(Client)

class Em3000;

class CLIENT_DLL Em3000Movement final : public Movement
{
public:
	typedef struct tagEm3000MovementDesc : public MOVEMENT_DESC
	{

	} EM3000_MOVEMENT_DESC;

public:
	typedef struct tagEm3000MovementData
	{
		Vector3 direction{ 0.f, 0.f, 0.f };
		Vector3 lookDirection{ 0.f, 0.f, 0.f };

		Bool isMove{ false };
		Bool isAttack{ false };
		Bool canRotation{ true };
		Bool useRootMotionDir{ true };
		Float rootMotionScale{ 1.f };
	} EM3000_MOVEMENT_DATA;

public:
	void Set_MovementData(const EM3000_MOVEMENT_DATA& data) { m_CurrentMoveData = data; Reset_RootMotionStop(); }
	const EM3000_MOVEMENT_DATA& Get_MovementData() const { return m_CurrentMoveData; }

public:
	Vector3 Get_RootPosition() const { return m_RootPosition; }
	void Set_RootPosition(const Vector3& rootPos) { m_RootPosition = rootPos; }

public:
	void Set_TargetPosition(const Vector3& targetPos) {
		m_TargetPosition = targetPos; m_HasWalkTarget = true;
	}
	Vector3 Get_TargetPosition() const { return m_TargetPosition; }
	void Clear_TargetPosition() { m_HasWalkTarget = false; }
	Bool Has_WalkTarget() const { return m_HasWalkTarget; }
	Bool Has_ReachedTarget(Float threshold = 1.0f) const;

public:
	explicit Em3000Movement();
	explicit Em3000Movement(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Em3000Movement(const Em3000Movement& rhs);
	~Em3000Movement() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg = nullptr) override;
	HRESULT Begin() override;

public:
	void Update_Movement(Float timeDelta) override;

private:
	EM3000_MOVEMENT_DATA m_CurrentMoveData{};
	Vector3 m_LastGroundedRootPositionVelocity{ 0.f, 0.f, 0.f };

private:
	Vector3 m_RootPosition{};
	Vector3 m_TargetPosition{};
	Bool m_HasWalkTarget{ false };

private:
	Weak<Navigation> m_Navigation{};

public:
	static Shared<Em3000Movement> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<Component> Clone(void* arg = nullptr) override;
};

NS_END
