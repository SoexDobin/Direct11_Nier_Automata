#pragma once
#include "Pl0000Parts.h"

NS_BEGIN(Client)

class CLIENT_DLL WP0070Body final : public Pl0000Parts
{
	RTTR_ENABLE(Pl0000Parts)
public:
	explicit WP0070Body();
	explicit WP0070Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit WP0070Body(const WP0070Body& rhs);
	~WP0070Body() override = default;

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
	static Shared<WP0070Body> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;

public:
	enum class WP0070_STATE {
		SHEATHE_LIGHT = 0,
		SHEATHE_HEAVY = 1,

	};
};

NS_END

/*
 wp0070_0000.mot > 0
wp0070_0001.mot > 1
wp0070_005a.mot > 2
wp0070_005b.mot > 3
wp0070_0100.mot > 4 (지상 1타)
wp0070_0101.mot > 5 (지상 2타)
wp0070_0102.mot > 6 (지상 3타)
wp0070_0103.mot > 7 (지상 4타)
wp0070_0104.mot > 8 (지상 5타)
wp0070_0105.mot > 9 (지상 6타)
wp0070_0106.mot > 10 (지상 7타)
wp0070_0107.mot > 11
wp0070_0108.mot > 12
wp0070_0110.mot > 13
wp0070_0111.mot > 14
wp0070_0113.mot > 15
wp0070_0114.mot > 16
wp0070_0116.mot > 17
wp0070_0117.mot > 18
wp0070_0119.mot > 19
wp0070_0130.mot > 20 (공중 1,3타)
wp0070_0131.mot > 21 (공중 2,4타)
wp0070_0132.mot > 22 (공중 5타)
wp0070_0133.mot > 23
wp0070_0134.mot > 24
wp0070_0135.mot > 25
wp0070_0136.mot > 26
wp0070_0150.mot > 27
wp0070_0155.mot > 28
wp0070_0156.mot > 29
wp0070_0157.mot > 30
wp0070_0160.mot > 31
wp0070_0180.mot > 32
wp0070_0188.mot > 33
wp0070_018d.mot > 34
wp0070_018e.mot > 35
wp0070_018f.mot > 36
wp0070_01e0.mot > 37
wp0070_01e1.mot > 38
wp0070_01e8.mot > 39
wp0070_01e9.mot > 40
wp0070_01f0.mot > 41
wp0070_01f5.mot > 42
wp0070_01f6.mot > 43
wp0070_01f7.mot > 44
wp0070_0600.mot > 45
wp0070_0640.mot > 46
wp0070_0643.mot > 47
wp0070_0644.mot > 48
wp0070_064c.mot > 49
wp0070_0655.mot > 50
wp0070_0660.mot > 51 (지상 Hold)
wp0070_0900.mot > 52
wp0070_0901.mot > 53
wp0070_0902.mot > 54
wp0070_0903.mot > 55
wp0070_0904.mot > 56
wp0070_0905.mot > 57
wp0070_0906.mot > 58
wp0070_0907.mot > 59
wp0070_0908.mot > 60
wp0070_0909.mot > 61
wp0070_090a.mot > 62
wp0070_090b.mot > 63
wp0070_c001.mot > 64
wp0070_c002.mot > 65
wp0070_e089.mot > 66
wp0070_e0e5.mot > 67
wp0070_e0e6.mot > 68
wp0070_e0e7.mot > 69
wp0070_e0e8.mot > 70
 */