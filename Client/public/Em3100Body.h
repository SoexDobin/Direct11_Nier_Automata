#pragma once
#include "PartObject.h"

NS_BEGIN(Engine)
	class AABBCollider;
	class Model;
NS_END

NS_BEGIN(Client)

class WP0220Body;
class WP0070Body;

class CLIENT_DLL Em3100Body final : public PartObject
{
	RTTR_ENABLE(PartObject)
public:
	typedef struct tagEm3100BodyDesc : public PARTOBJECT_DESC
	{
	} EM3100BODY_DESC;

public:
	explicit Em3100Body();
	explicit Em3100Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Em3100Body(const Em3100Body& rhs);
	~Em3100Body() override = default;

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

private:
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Components();

private:
	Shared<Shader>	m_Shader{ nullptr };
	Shared<Model>	m_Model{ nullptr };
	Shared<AABBCollider> m_HitBox{ nullptr };

public:
	static Shared<Em3100Body> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END
