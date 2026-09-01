#pragma once
#include "ParticleEffect.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Particle_Point;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL FireFlashEffect final : public ParticleEffect
{
public:
	typedef struct tagFireFlashEffectDesc : public PARTICLE_EFFECT_DESC
	{
		Matrix parentMatrix{};
	} FIRE_FLASH_EFFECT_DESC;

public:
	explicit FireFlashEffect();
	explicit FireFlashEffect(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit FireFlashEffect(const FireFlashEffect& rhs);
	~FireFlashEffect() override = default;
	
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
	HRESULT Ready_Components(const Matrix& parentMatrix);
	HRESULT Bind_ShaderResources();

private:
	Shared<Shader> m_Shader{ nullptr };
	Shared<Texture> m_Texture{ nullptr };
	Shared<VIBuffer_Particle_Point>  m_Buffer{ nullptr };
	Float m_Acc{ 0.f };

public:
	static Shared<FireFlashEffect> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END
