#include "AABBCollider.h"
#include "SphereCollider.h"
#include "OBBCollider.h"
#include "Game.h"
#include "SpdLogger.h"

#ifdef _DEBUG
#include "DebugDraw.h"
#endif

OBBCollider::OBBCollider() : Collider{} {}
OBBCollider::OBBCollider(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Collider{ device, context } {
}
OBBCollider::OBBCollider(const OBBCollider& rhs)
	: Collider(rhs), m_Original(rhs.m_Original), m_Transformed(rhs.m_Transformed) {
}

HRESULT OBBCollider::Initialize_Prototype()
{
	return Collider::Initialize_Prototype();
}
HRESULT OBBCollider::Initialize(void* arg)
{
	if (FAILED(Collider::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize OBBCollider");
		return E_FAIL;
	}

	if (arg != nullptr)
	{
		auto desc = *static_cast<OBB_COLLIDER_DESC*>(arg);
		m_Original.Center = desc.offset;
		m_Original.Extents = desc.extents;
		m_Offset = desc.offset;

		Quaternion rotation = Quaternion::CreateFromYawPitchRoll(desc.rotation.y, desc.rotation.x, desc.rotation.z);
		m_Original.Orientation = rotation;
		m_Offset = desc.offset;
	}
	else
	{
		m_Original.Center = m_Offset;
		m_Original.Extents = Vector3(0.5f, 0.5f, 0.5f);
		m_Original.Orientation = Quaternion::Identity;
	}

	m_Transformed = m_Original;
	return S_OK;
}
void OBBCollider::Update(const Matrix& worldMatrix)
{
	m_Original.Transform(m_Transformed, worldMatrix);
}
Bool OBBCollider::Intersect(const Shared<Collider>& target)
{
	if (target == nullptr) return false;
	switch (target->Get_ColliderType())
	{
	case COLLIDER_TYPE::SPHERE:
		return m_Transformed.Intersects(static_pointer_cast<SphereCollider>(target)->Get_Bounding());
	case COLLIDER_TYPE::AABB:
		return m_Transformed.Intersects(static_pointer_cast<AABBCollider>(target)->Get_Bounding());
	case COLLIDER_TYPE::OBB:
		return m_Transformed.Intersects(static_pointer_cast<OBBCollider>(target)->Get_Bounding());
	default:
		return false;
	}
}

#ifdef _DEBUG
HRESULT OBBCollider::Render_Debug(const Shared<PrimitiveBatch<VertexPositionColor>>& batch, const Color& color)
{
	DX::Draw(batch.get(), m_Transformed, color);
	return S_OK;
}
#endif

Shared<Component> OBBCollider::Clone(void* arg)
{
	auto clone = make_shared<OBBCollider>(*this);
	if (FAILED(clone->Initialize(arg))) return nullptr;

	GAME_INSTANCE->Add_Collider(clone);

	return clone;
}
Shared<OBBCollider> OBBCollider::CreatePrototype()
{
	auto instance = make_shared<OBBCollider>();
	if (FAILED(instance->Initialize_Prototype())) return nullptr;
	return instance;
}
Shared<OBBCollider> OBBCollider::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	auto instance = make_shared<OBBCollider>(device, context);
	if (FAILED(instance->Initialize_Prototype())) return nullptr;
	return instance;
}