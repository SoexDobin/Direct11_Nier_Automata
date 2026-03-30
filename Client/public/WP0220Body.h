#pragma once
#include "Pl0000Parts.h"

NS_BEGIN(Client)

class CLIENT_DLL WP0220Body final : public Pl0000Parts
{
	RTTR_ENABLE(Pl0000Parts)
public:
	explicit WP0220Body();
	explicit WP0220Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit WP0220Body(const WP0220Body& rhs);
	~WP0220Body() override = default;

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

public:
	static Shared<WP0220Body> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;

public:
	enum class WP0220_STATE {
		SHEATHE_LIGHT = 0,
		SHEATHE_HEAVY = 1,

	};
};

NS_END

/*
wp0220_0000.mot > 0
wp0220_005a.mot > 1
wp0220_005b.mot > 2
wp0220_0200.mot > 3
wp0220_0201.mot > 4
wp0220_0202.mot > 5
wp0220_0203.mot > 6
wp0220_0205.mot > 7 (지상 1타)
wp0220_0207.mot > 8
wp0220_0209.mot > 9 (지상 2타)
wp0220_020b.mot > 10
wp0220_020d.mot > 11 (지상 3타)
wp0220_020f.mot > 12
wp0220_0211.mot > 13 (Hold_Unfull)
wp0220_0212.mot > 14 (Hold_Cycle)
wp0220_0213.mot > 15 (Hold_NoContact)
wp0220_0214.mot > 16 (Hold_Full)
wp0220_021a.mot > 17
wp0220_0220.mot > 18
wp0220_0225.mot > 19 (공중 진입)
wp0220_0226.mot > 20 (공중 Hold)
wp0220_0227.mot > 21 (공중 낙하)
wp0220_022a.mot > 22
wp0220_0280.mot > 23
wp0220_0288.mot > 24
wp0220_028d.mot > 25
wp0220_028e.mot > 26
wp0220_028f.mot > 27
wp0220_02e0.mot > 28
wp0220_02e1.mot > 29
wp0220_02e8.mot > 30
wp0220_02f0.mot > 31
wp0220_02f5.mot > 32
wp0220_02f6.mot > 33
wp0220_02f7.mot > 34
wp0220_0600.mot > 35
wp0220_0640.mot > 36
wp0220_0646.mot > 37
wp0220_0647.mot > 38
wp0220_064f.mot > 39
wp0220_0658.mot > 40
wp0220_0659.mot > 41
wp0220_0663.mot > 42
wp0220_c201.mot > 43
wp0220_c202.mot > 44
wp0220_c234.mot > 45
 */