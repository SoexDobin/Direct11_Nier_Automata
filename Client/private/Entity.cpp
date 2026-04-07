#include "pch.h"
#include "Entity.h"

#include <AABBCollider.h>
#include <Collider.h>
#include <OBBCollider.h>
#include <SphereCollider.h>

#include "SpdLogger.h"

Entity::Entity() : ContainerObject{} {}
Entity::Entity(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: ContainerObject{device, context} {}
Entity::Entity(const Entity& rhs)
	: ContainerObject{rhs} {}

void Entity::TakeDamage(const DAMAGE_INFO& dmgInfo)
{
	if (m_IsInvincible) return;
	if (Is_Dead()) return;
	m_Hp -= dmgInfo.damage;
	if (m_Hp <= 0.f)
	{
		m_Hp = 0.f;
		OnDeath();
	}
}

Vector3 Entity::PushoutDelta(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider, Float ratio)
{
	Vector3 pushDir;
	Float depth;

	if (Calc_Penetration(ownCollider, targetCollider, pushDir, depth))
	{
		return pushDir * (depth * ratio);
	}

	return Vector3::Zero;
}

void Entity::Pullout(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider, Float ratio)
{
	Vector3 pushDir;
	Float depth;

	if (Calc_Penetration(ownCollider, targetCollider, pushDir, depth))
	{
		auto targetTransform = targetCollider->Get_Owner()->Get_Transform();
		if (!targetTransform) return;

		Vector3 newPos = targetTransform->Get_Position() + (-pushDir * (depth * ratio));
		newPos.y = targetTransform->Get_Position().y;
		targetTransform->Set_Position(newPos);
	}
}

Bool Entity::Calc_Penetration(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider, Vector3& outDir, Float& outDepth)
{
	if (!ownCollider || !targetCollider) return false;

	Vector3 myPos = ownCollider->Get_Pivot();
	Vector3 targetPos = targetCollider->Get_Pivot();

	outDir = myPos - targetPos;
	outDir.y = 0.f;

	Float distance = outDir.Length();
	if (distance > 0.001f)
		outDir.Normalize();
	else
		outDir = Vector3{ 1.f, 0.f, 0.f };

	Float myRadius = Get_RadiusByColliderType(ownCollider, outDir);
	Float targetRadius = Get_RadiusByColliderType(targetCollider, -outDir);

	Float minAllowedDistance = myRadius + targetRadius;

	if (distance < minAllowedDistance)
	{
		outDepth = minAllowedDistance - distance;
		return true;
	}

	return false;
}


Float Entity::Get_RadiusByColliderType(const Shared<Collider>& collider, const Vector3 colDirection)
{
	auto colType = collider->Get_ColliderType();
	if (colType == COLLIDER_TYPE::SPHERE)
	{
		return static_pointer_cast<SphereCollider>(collider)->Get_Radius();
	}
	if (colType == COLLIDER_TYPE::AABB)
	{
		Vector3 absDirection = Vector3{ fabs(colDirection.x), fabs(colDirection.y), fabs(colDirection.z) };
		return absDirection.Dot(static_pointer_cast<AABBCollider>(collider)->Get_CurrentExtends());
	}
	if (colType == COLLIDER_TYPE::OBB)
	{
#ifdef _DEBUG
		//LOG_DEBUG(L"Use OBB Collider Compare Are You Sure?");
#endif
		auto obb = static_pointer_cast<OBBCollider>(collider);

		Quaternion invQuat{}; 
		obb->Get_CurrentOrientation().Inverse(invQuat);
		Vector3 localDir = Vector3::TransformNormal(colDirection, Matrix::CreateFromQuaternion(invQuat));

		Vector3 absDirection = Vector3{ fabs(localDir.x), fabs(localDir.y), fabs(localDir.z) };
		return absDirection.Dot(obb->Get_CurrentExtends());
	}

	LOG_ERROR(L"Wrong Collider Type");
	return 0.f;
}
