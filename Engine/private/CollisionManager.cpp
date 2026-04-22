#include "CollisionManager.h"
#include "Collider.h"
#include "Game.h"
#include "GameObject.h"

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
	m_Colliders.clear();
#ifdef _DEBUG
	m_Batch.reset();
	m_Effect.reset();
	m_InputLayout.Reset();
	m_Device.Reset();
	m_Context.Reset();
#endif
}

HRESULT CollisionManager::Initialize_Prototype(uint32 levCount)
{
	m_LevelCount = levCount;
	m_Colliders.resize(m_LevelCount);
	return EngineManager::Initialize_Prototype();
}

HRESULT CollisionManager::Initialize(void* arg)
{
	return EngineManager::Initialize(arg);
}

void CollisionManager::On_Destroy() { m_Colliders.clear(); }
void CollisionManager::On_Disable() { EngineManager::On_Disable(); }
void CollisionManager::On_Enable() { EngineManager::On_Enable(); }
void CollisionManager::Set_Active(Bool isActive) { EngineManager::Set_Active(isActive); }

void CollisionManager::Add_Collider(uint32 levIndex, const Shared<Collider>& collider)
{
	if (levIndex >= m_LevelCount) return;

	m_Colliders[levIndex].push_back(collider);
}

void CollisionManager::Remove_Collider(uint32 levIndex, const Shared<Collider>& collider)
{
	if (nullptr == collider || levIndex >= m_LevelCount) return;

	for (auto& col : m_Colliders[levIndex])
	{
		if (col && col != collider)
			col->Release_OverlapMember(collider);
	}

	auto iter = ranges::find_if(m_Colliders[levIndex].begin(), m_Colliders[levIndex].end(), [collider](const Shared<Collider>& col) {
		return col == collider;
	});
	if (iter != m_Colliders[levIndex].end())
	{
		m_Colliders[levIndex].erase(iter);
	}
}

HRESULT CollisionManager::Clear_Colliders(uint32 levIndex)
{
	if (levIndex >= m_LevelCount)
	{
		return E_FAIL;
	}
	m_Colliders[levIndex].clear();
	return S_OK;
}

void CollisionManager::Update_Collision() const
{
	uint32 levIndex = GAME_INSTANCE->Get_CurrentLevelIndex();

	for (size_t i = 0; i < m_Colliders[levIndex].size(); ++i)
	{
		auto srcCol = m_Colliders[levIndex][i];
		if (!srcCol || srcCol->Is_Destroy() || !srcCol->Is_Active()) continue;
		auto srcOwner = srcCol->Get_Owner();
		if (!srcOwner || srcOwner->Is_Destroy() || !srcOwner->Is_Active()) continue;

		for (size_t j = i + 1; j < m_Colliders[levIndex].size(); ++j)
		{
			auto dstCol = m_Colliders[levIndex][j];
			if (!dstCol || dstCol->Is_Destroy() || !dstCol->Is_Active()) continue;
			auto dstOwner = dstCol->Get_Owner();
			if (!dstOwner || dstOwner->Is_Destroy() || !dstOwner->Is_Active()) continue;

			// --- Layer Mask 필터링 ---
			uint32 srcLayerBit = srcOwner->Get_LayerMask().Get_Layer();
			uint32 dstLayerBit = dstOwner->Get_LayerMask().Get_Layer();
			
			uint32 srcMaskBits = srcOwner->Get_LayerMask().Is_GlobalMask() ?
				GAME_INSTANCE->Get_LayerRegister()->Get_GlobalMask(srcLayerBit)
				: srcOwner->Get_LayerMask().Get_Mask();
			uint32 dstMaskBits = dstOwner->Get_LayerMask().Is_GlobalMask() ?
				GAME_INSTANCE->Get_LayerRegister()->Get_GlobalMask(dstLayerBit)
				: dstOwner->Get_LayerMask().Get_Mask();

			if ((srcMaskBits & dstLayerBit) == 0 || (dstMaskBits & srcLayerBit) == 0)
				continue;
			// -------------------------

			if (Bool isIntersecting = srcCol->Intersect(dstCol))
			{
				srcCol->Set_IsColliding(true);
				dstCol->Set_IsColliding(true);

				if (!srcCol->Is_Overlapped(dstCol))
				{
					srcCol->Add_OverlapMember(dstCol);
					srcOwner->OnCollisionEnter(srcCol, dstCol);

					dstCol->Add_OverlapMember(srcCol);
					dstOwner->OnCollisionEnter(dstCol, srcCol);
				}
				else
				{
					srcOwner->OnCollisionStay(srcCol, dstCol);
					dstOwner->OnCollisionStay(dstCol, srcCol);
				}
			}
			else
			{
				if (srcCol->Is_Overlapped(dstCol))
				{
					srcCol->Release_OverlapMember(dstCol);
					srcOwner->OnCollisionExit(srcCol, dstCol);

					dstCol->Release_OverlapMember(srcCol);
					dstOwner->OnCollisionExit(dstCol, srcCol);
				}
			}
		}
	}
}

#ifdef _DEBUG
HRESULT CollisionManager::Initialize_Debug()
{
	m_Device = GAME_INSTANCE->Get_Device();
	m_Context = GAME_INSTANCE->Get_Context();

	m_Batch = make_shared<PrimitiveBatch<VertexPositionColor>>((m_Context.Get()));
	m_Effect = make_shared<BasicEffect>(m_Device.Get());

	m_Effect->SetVertexColorEnabled(true);

	const void* shaderByteCode = nullptr;
	size_t shaderByteCodeLength = 0;

	m_Effect->GetVertexShaderBytecode(&shaderByteCode, &shaderByteCodeLength);
	if (FAILED(m_Device->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, shaderByteCode, shaderByteCodeLength, &m_InputLayout)))
		return E_FAIL;
	return S_OK;
}

void CollisionManager::Render_Debug() const
{
	if (!m_DebugMode) return;
	if (!m_Effect || !m_Batch) return;

	m_Effect->SetWorld(Matrix::Identity);
	m_Effect->SetView(GAME_INSTANCE->Get_Transform(D3DTS::VIEW));
	m_Effect->SetProjection(GAME_INSTANCE->Get_Transform(D3DTS::PROJ));

	m_Context->GSSetShader(nullptr, nullptr, 0);
	m_Context->IASetInputLayout(m_InputLayout.Get());

	m_Effect->Apply(m_Context.Get());

	m_Batch->Begin();

	uint32 levIndex = GAME_INSTANCE->Get_CurrentLevelIndex();
	for (auto& collider : m_Colliders[levIndex])
	{
		if (collider && collider->Is_Active() && !collider->Is_Destroy())
		{
			Bool colliding = collider->Get_IsColliding();
			FXMVECTOR color = colliding ? DirectX::Colors::Red : DirectX::Colors::Green;
			collider->Render_Debug(m_Batch, color);
			collider->Set_IsColliding(false);
		}
	}
	m_Batch->End();
}

#endif

Unique<CollisionManager> CollisionManager::Create(uint32 levCount)
{
	auto collisionManager = make_unique<CollisionManager>();

#ifdef _DEBUG
	if (collisionManager->Initialize_Debug())
	{
		MSG_BOX("Failed to Initialize Debug CollisionManager");
		return nullptr;
	}
#endif

	if (collisionManager->Initialize_Prototype(levCount))
	{
		MSG_BOX("Failed to Create CollisionManager");
		return nullptr;
	}

	return collisionManager;
}
