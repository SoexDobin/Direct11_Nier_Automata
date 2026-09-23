#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
	class Shader;
	class Model;
	class Navigation;
	class WorldCollider;
NS_END

NS_BEGIN(Client)

/// 월드 타일 하나. 어떤 타일인지는 저장된 Model의 ModelTag가 정하고,
/// 충돌(<태그>_COL)과 길찾기(<태그>.nnav)는 Post_Load에서 그 태그를 따라간다.
class CLIENT_DLL WorldMap final : public GameObject
{
public:
	explicit WorldMap();
	explicit WorldMap(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit WorldMap(const WorldMap& rhs);
	virtual ~WorldMap() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;
	void On_Enable() override;
	void On_Disable() override;

public:
	void Priority_Update(Float timeDelta) override {}
	void Update(Float timeDelta) override {}
	void Late_Update(Float timeDelta) override {}
	void Fixed_Update(Float fixedDelta) override {}
	HRESULT Render() override;
	void Submit_RenderGroup() override;

protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	/// 이번 프레임에 그릴 모델을 고른다. 멀면 <ModelTag>_LOD, 가까우면 원본이다.
	const Shared<Model>& Select_Model();

private:
	Shared<Shader> m_Shader{ nullptr };
	Shared<Model> m_Model{ nullptr };
	// <ModelTag>_LOD 프로토타입. 등록돼 있지 않으면 nullptr로 남고 LOD는 그냥 꺼진다.
	Shared<Model> m_LodModel{ nullptr };
	Bool m_LodResolved{ false };
	Bool m_UseLod{ false };
	Shared<Navigation> m_Navigation{ nullptr };
	Shared<WorldCollider> m_WorldCollider{ nullptr };

public:
	static Shared<WorldMap> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END
