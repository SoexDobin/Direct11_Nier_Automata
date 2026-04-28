#pragma once
#include "ParticleEffect.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Particle_Point;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL SparkEffect final : public ParticleEffect
{
	RTTR_ENABLE(ParticleEffect)
public:
	typedef struct tagSparkEffectDesc : public PARTICLE_EFFECT_DESC
	{
		ATK_TYPE atkType{};
		Vector3 position{};
		Quaternion rotation{ 0.f, 0.f, 0.f, 1.f };
		Float threshold{ 0.005f }; // 프레임당 유지 시간
	} SPARK_EFFECT_DESC;

public:
	explicit SparkEffect();
	explicit SparkEffect(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit SparkEffect(const SparkEffect& rhs);
	~SparkEffect() override = default;
	
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override { ParticleEffect::On_Destroy(); }
	void On_Enable() override { ParticleEffect::On_Enable(); }
	void On_Disable() override { ParticleEffect::On_Disable(); }

public:
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void Fixed_Update(Float fixedDelta) override;
	void Submit_RenderGroup() override;
	HRESULT Render() override;

private:
	HRESULT Ready_Components(ATK_TYPE atkType, const Vector3& initialPosition, const Quaternion& initialRotation);
	HRESULT Bind_ShaderResources();

private:
	Shared<Shader> m_Shader{ nullptr };
	Shared<Texture> m_Texture{ nullptr };
	Shared<VIBuffer_Particle_Point>  m_Buffer{ nullptr };
	Float m_Acc{ 0.f };
	Float m_DiscardBlack{ 0.f };
	uint32 m_CurIndex{ 0 };
	Float m_ThreshHold{ 0.01f };

public:
	static Shared<SparkEffect> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END
