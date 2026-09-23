#pragma once
#include "StaticMesh.h"

NS_BEGIN(Engine)

// 같은 정적 지오메트리를 인스턴스 변환 행렬만 바꿔 여러 번 그린다.
// 현재는 클래스 경계와 draw 경로까지만 있고 실제 사용처는 없다.
// 인스턴스 입력 파이프라인(LAY 배치 등)은 별도 단계다.
class InstanceMesh final : public StaticMesh
{
	RTTR_ENABLE(StaticMesh)
public:
	explicit InstanceMesh();
	explicit InstanceMesh(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit InstanceMesh(const InstanceMesh& rhs);
	~InstanceMesh() override = default;

public:
	// 인스턴스별 월드 행렬. 개수가 늘면 버퍼를 다시 만들고, 아니면 덮어쓴다.
	HRESULT Set_Instances(const vector<Matrix>& worldMatrices);
	uint32 Get_NumInstances() const { return m_NumInstances; }

public:
	HRESULT Bind_Resources() override;
	HRESULT Render() override;
	void On_Destroy() override;

private:
	ComPtr<ID3D11Buffer> m_VBInstance{ nullptr };
	uint32 m_InstanceStride{ sizeof(Matrix) };
	uint32 m_NumInstances{};
	uint32 m_InstanceCapacity{};

public:
	Shared<Component> Clone(void* arg = nullptr) override { return nullptr; };
};

NS_END
