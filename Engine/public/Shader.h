#pragma once
#include "Component.h"

#include <type_traits>

NS_BEGIN(Engine)

class ENGINE_DLL Shader final : public Component, public enable_shared_from_this<Shader> {
    RTTR_ENABLE(Component)

public:
    typedef struct tagShaderDesc : public COMPONENT_DESC
    {
        tagShaderDesc(){}
        tagShaderDesc(const wstring& vertextag, const D3D11_INPUT_ELEMENT_DESC* desc, uint32_t numPasses)
	        : m_VertexTag(vertextag), m_elementsDesc(desc), m_numPasses(numPasses) {}

        wstring m_VertexTag{};
        const D3D11_INPUT_ELEMENT_DESC* m_elementsDesc{nullptr};
        uint32_t m_numPasses{};
    } SHADER_DESC;
public:
    explicit Shader();
    explicit Shader(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context);
    explicit Shader(const Shader& rhs);
    ~Shader() override = default;

public:
    COMPONENT_TYPE Get_ComponentType() const override { return COMPONENT_TYPE::SHADER; };
    HRESULT Initialize_Prototype(const tChar *shaderFilePath,
                                 const D3D11_INPUT_ELEMENT_DESC* elements,
                                 uint32 numElements);
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void *arg) override;
    void On_Destroy() override;

public:
    HRESULT Begin(uint32 passIndex);
    HRESULT Bind_SRV(const Char* constantName, const ComPtr<ID3D11ShaderResourceView> &srv);
    HRESULT Bind_Matrix(const Char* constantName, const Float4x4 *matrix);
    HRESULT Bind_Matrices(const Char* constantName, const Float4x4* matrices, uint32 numMatrices);
    HRESULT Bind_RawValue(const Char* constantName, const void* data, uint32 length);
    Bool Supports_CBuffer(ConstantBuffer type) const;

    template <typename Desc>
    HRESULT Bind_CBufferData(const Desc& desc)
    {
        static_assert(sizeof(Desc) % 16 == 0,
            "Constant buffer data size must be 16-byte aligned.");
        static_assert(std::is_trivially_copyable_v<Desc>,
            "Constant buffer data must be trivially copyable.");

        return Update_CBuffer(Desc::CBType, &desc, sizeof(Desc));
    }

private:
    HRESULT Update_CBuffer(ConstantBuffer type, const void* data, size_t size);

private:
    ComPtr<ID3DX11Effect> m_Effect = { nullptr };
    uint32 m_NumPasses = {};
    vector<ComPtr<ID3D11InputLayout>> m_InputLayouts;
    array<ComPtr<ID3DX11EffectConstantBuffer>, ETOI(ConstantBuffer::END)> m_ConstantBuffers{};
    array<ComPtr<ID3D11Buffer>, ETOI(ConstantBuffer::END)> m_AllocatedConstantBuffers{};
    array<uint32, ETOI(ConstantBuffer::END)> m_AllocatedConstantBufferSizes{};

public:
    static Shared<Shader> CreatePrototype();
	static Shared<Shader> Create(const ComPtr<ID3D11Device>& device,
                               const ComPtr<ID3D11DeviceContext>& context,
                               const tChar* shaderFilePath,
                               const D3D11_INPUT_ELEMENT_DESC* elements,
                               uint32 numElements);
	Shared<Component> Clone(void* arg) override;
};

NS_END
