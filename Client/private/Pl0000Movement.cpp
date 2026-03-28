#include "pch.h"
#include "Pl0000Movement.h"

#include <Game.h>
#include <SpdLogger.h>
#include "Camera.h"
#include "Pl0000Input.h"
#include "Pl0000.h"

Pl0000Movement::Pl0000Movement() : Movement{} {}
Pl0000Movement::Pl0000Movement(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Movement{device, context} {}
Pl0000Movement::Pl0000Movement(const Pl0000Movement& rhs) : Movement{rhs} {}

HRESULT Pl0000Movement::Initialize_Prototype()
{
	return Movement::Initialize_Prototype();
}

HRESULT Pl0000Movement::Initialize(void* arg)
{
	if (FAILED(Movement::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Init {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Pl0000Movement::Begin()
{
	if (!m_Owner.expired())
	{
		m_OwnerContainer = static_pointer_cast<Pl0000>(m_Owner.lock());
		if (m_OwnerContainer.expired())
		{
			LOG_ERROR(L"Failed To Find Pl0000 Container");
			return E_FAIL;
		}

		m_Input = m_Owner.lock()->Get_Component<Pl0000Input>();
		if (m_Input.expired())
		{
			LOG_ERROR(L"Failed To Find Pl0000Input");
			return E_FAIL;
		}

		GAME_INSTANCE->Add_Instance_Event(ETOI(LEVEL::GAMEPLAY), L"Add_Owner_To_Camera", [&]()
			{
				auto camera = GAME_INSTANCE->Get_MainCamera();
				LOG_DEBUG(L"Catch MainCamera {}", camera->Get_Name());
				if (camera != nullptr)
				{
					camera->Set_Target(m_Owner.lock());
					m_TargetCamera = camera;
				}
			});
	}

	return S_OK;
}

void Pl0000Movement::Update_Movement(Float timeDelta)
{
	if (m_Owner.expired() || m_Input.expired()) return;
	if (m_TargetCamera.expired()) return;

	Shared<Transform> ownerTransform = m_Owner.lock()->Get_Transform();
	if (ownerTransform == nullptr) return;
	
	Vector3 inputDir = Vector3::Zero;
	if (GAME_INSTANCE->Get_DIKeyState(UBYTE(DIKEYBOARD_W)) & 0x80) inputDir.z += 1.f;
	if (GAME_INSTANCE->Get_DIKeyState(UBYTE(DIKEYBOARD_S)) & 0x80) inputDir.z -= 1.f;
	if (GAME_INSTANCE->Get_DIKeyState(UBYTE(DIKEYBOARD_A)) & 0x80) inputDir.x -= 1.f;
	if (GAME_INSTANCE->Get_DIKeyState(UBYTE(DIKEYBOARD_D)) & 0x80) inputDir.x += 1.f;

	if (inputDir.LengthSquared() > 0.f)
	{
		inputDir.Normalize();
		auto cameraTransform = m_TargetCamera.lock()->Get_Transform();
		Vector3 camLook = cameraTransform->Get_Look();
		Vector3 camRight = cameraTransform->Get_Right();

		camLook.y = 0.f; camLook.Normalize();
		camRight.y = 0.f; camRight.Normalize();

		Vector3 targetDir = (camLook * inputDir.z) + (camRight * inputDir.x);
		targetDir.Normalize();

		Float targetYaw = atan2f(targetDir.x, targetDir.z);
		Quaternion targetQuat = Quaternion::CreateFromYawPitchRoll(targetYaw, 0.f, 0.f);

		Float turnSpeed = m_TurnSpeed * timeDelta;
		Quaternion currentQuat = ownerTransform->Get_Quaternion();
		
		Quaternion nextQuat = Quaternion::Slerp(currentQuat, targetQuat, turnSpeed);
		ownerTransform->Set_Rotation(nextQuat);
		
		TRANSFORM_FRAME transformFrame = m_OwnerContainer.lock()->Get_BodyModelTransform();

		Vector3 worldMoveDelta = Vector3::Transform(transformFrame.position * -1.f, nextQuat);
		ownerTransform->Set_Position(ownerTransform->Get_Position() + worldMoveDelta * timeDelta);
	}

	//if (inputDir.LengthSquared() > 0.f)
	//{
	//	inputDir.Normalize();
	//
	//	auto camera = GAME_INSTANCE->Get_MainCamera();
	//	Vector3 camLook = camera->Get_Transform()->Get_Look();
	//	Vector3 camRight = camera->Get_Transform()->Get_Right();
	//
	//	camLook.y = 0.f; camLook.Normalize();
	//	camRight.y = 0.f; camRight.Normalize();
	//
	//	Vector3 targetDir = (camLook * inputDir.z) + (camRight * inputDir.x);
	//	targetDir.Normalize();
	//
	//	// 3. 캐릭터의 현재 회전값을 목표 방향으로 부드럽게 회전 (Slerp 활용)
	//	float targetYaw = atan2f(targetDir.x, targetDir.z); // XZ 평면에서의 회전 각도(Yaw)
	//	Quaternion targetQuat = Quaternion::CreateFromYawPitchRoll(targetYaw, 0.f, 0.f);
	//
	//	// turnSpeed가 작을수록 크게 둥글게 돌고, 클수록 좁게 휙 돕니다. (원하는 조작감에 맞춰 수정)
	//	float turnSpeed = 8.0f * timeDelta;
	//	Quaternion currentQuat = m_Transform->Get_Quaternion();
	//	Quaternion nextQuat = Quaternion::Slerp(currentQuat, targetQuat, turnSpeed);
	//
	//	// 회전 적용
	//	m_Transform->Set_Rotation(nextQuat);
	//
	//	// 4. ★핵심: 이동 방향(dir)은 targetDir이 아니라 방금 회전한 캐릭터의 현재 Look 방향!
	//	Vector3 currentLook = m_Transform->Get_Look();
	//	currentLook.y = 0.f;
	//	currentLook.Normalize();
	//
	//	m_LastDirection = currentLook;
	//
	//	// (옵션) 기존 루트 모션 방식 사용 시
	//	TRANSFORM_FRAME transformFrame = m_Model->Get_RootTransformVelocity(m_RootBoneIndex);
	//	Vector3 worldMoveDelta = Vector3::Transform(transformFrame.position * -1.f, nextQuat);
	//	m_Transform->Set_Position(m_Transform->Get_Position() + worldMoveDelta * timeDelta);
	//}

}

Shared<Pl0000Movement> Pl0000Movement::Create(const ComPtr<ID3D11Device>& device,
                                              const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Pl0000Movement>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Pl0000Movement");
		return nullptr;
	}

	return prototype;
}

Shared<Component> Pl0000Movement::Clone(void* arg)
{
	auto instance = make_shared<Pl0000Movement>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Pl0000Movement");
		return nullptr;
	}

	return instance;
}


