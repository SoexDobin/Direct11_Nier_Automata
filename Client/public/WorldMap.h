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
	/// 이번 프레임에 그릴 모델을 고른다. 거리에 따라 원본 / LOD1 / LOD2다.
	const Shared<Model>& Select_Model();
	/// <ModelTag>_LOD를 잡고 그 안에 섞여 있는 LOD1·LOD2를 밴드별 인덱스로 나누고, LOD 판이 없는
	/// 원본 메시를 골라둔다. 최초 1회만 돈다.
	void Ready_LodBands();

private:
	Shared<Shader> m_Shader{ nullptr };
	Shared<Model> m_Model{ nullptr };
	// <ModelTag>_LOD 프로토타입. 등록돼 있지 않으면 nullptr로 남고 LOD는 그냥 꺼진다.
	Shared<Model> m_LodModel{ nullptr };
	Bool m_LodResolved{ false };
	/* _LOD.model 하나에 LOD1과 LOD2가 함께 들어 있고 구분은 메시 이름뿐이다. 두 레벨을 같이
	   그리면 같은 물체가 겹치므로, 해석할 때 밴드별 인덱스를 만들어 한 밴드만 그린다. */
	vector<uint32> m_LodBand1;   // LOD1 메시
	vector<uint32> m_LodBand2;   // LOD2 메시 + LOD2가 없는 그룹의 LOD1 메시
	/* 원작은 물체 단위로 LOD를 바꾸고 LOD 판이 없는 물체(지면 등)는 원본을 계속 그린다.
	   _LOD.model에 같은 그룹이 없는 원본 메시 인덱스이며, 밴드1·2에서 LOD 메시와 함께 그린다. */
	vector<uint32> m_LodKeepOriginal;
	uint32 m_LodBand{ 0 };       // 0 = 원본, 1 = LOD1, 2 = LOD2
	/* 히스테리시스는 같은 카메라로 이어질 때만 뜻이 있다. Editor는 View마다 메인 카메라를
	   바꿔 끼우므로(EditorManager.cpp:969, :982) 어느 카메라의 밴드인지 같이 들고 있어야
	   한 View가 다른 View의 밴드를 물려받지 않는다. 식별용이며 역참조하지 않는다. */
	const class Camera* m_LodBandCamera{ nullptr };
	Shared<Navigation> m_Navigation{ nullptr };
	Shared<WorldCollider> m_WorldCollider{ nullptr };

public:
	static Shared<WorldMap> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END
