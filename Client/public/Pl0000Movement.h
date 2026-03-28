#pragma once
#include "Movement.h"

NS_BEGIN(Engine)
class Camera;
class Transform;
NS_END

NS_BEGIN(Client)

class Pl0000;
class Pl0000Input;

class CLIENT_DLL Pl0000Movement final : public Movement
{
	RTTR_ENABLE(Movement)
public:
	typedef struct tagPl0000MovementDesc : public MOVEMENT_DESC
	{

	} PL0000_MOVEMENT_DESC;

public:
	explicit Pl0000Movement();
	explicit Pl0000Movement(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Pl0000Movement(const Pl0000Movement& rhs);
	~Pl0000Movement() override = default;
	
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg = nullptr) override;
	HRESULT Begin() override;

public:
	void Update_Movement(Float timeDelta) override;

private:
	Weak<Pl0000> m_OwnerContainer{};
	Weak<Camera> m_TargetCamera{};
	Weak<Pl0000Input> m_Input{};

public:
	static Shared<Pl0000Movement> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<Component> Clone(void* arg = nullptr) override;
};

NS_END