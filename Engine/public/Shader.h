#pragma once
#include "Component.h"

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
    HRESULT Bind_RawValue(const Char* constantName, const void* data, uint32 length);

private:
    ComPtr<ID3DX11Effect> m_Effect = { nullptr };
    uint32 m_NumPasses = {};
    vector<ComPtr<ID3D11InputLayout>> m_InputLayouts;

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