#include "Shader.h"
#include "Game.h"
#include "SpdLogger.h"

Shader::Shader() : Component{} {}
Shader::Shader(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
    : Component(device, context) {}

Shader::Shader(const Shader& rhs)
    : Component(rhs), m_Effect{rhs.m_Effect}, m_NumPasses{rhs.m_NumPasses},
      m_InputLayouts{rhs.m_InputLayouts}, m_ConstantBuffers{rhs.m_ConstantBuffers} {}

HRESULT Shader::Initialize_Prototype(const tChar* shaderFilePath, const D3D11_INPUT_ELEMENT_DESC* elements, uint32 numElements) 
{
    uint32 hlslFlag = {};

    if constexpr (_DEBUG)
        hlslFlag |= D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
    else
		hlslFlag |= D3DCOMPILE_OPTIMIZATION_LEVEL1;

	if (FAILED(D3DX11CompileEffectFromFile(
        shaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, hlslFlag,
        0, m_Device.Get(), m_Effect.GetAddressOf(), nullptr)))
		return E_FAIL;

    if (ID3DX11EffectTechnique* technique = m_Effect->GetTechniqueByIndex(0)) 
    {
        Shader::SHADER_DESC desc;
    	D3DX11_TECHNIQUE_DESC techniqueDesc = {};
    	
    	technique->GetDesc(&techniqueDesc);
        m_NumPasses = techniqueDesc.Passes;
        m_InputLayouts.reserve(m_NumPasses);

        for (uint32 i = 0; i < m_NumPasses; ++i) 
        {
            ComPtr<ID3D11InputLayout> inputLayout = {nullptr};
            ID3DX11EffectPass* pass = technique->GetPassByIndex(i);
            if (nullptr == pass)
				return E_FAIL;

            D3DX11_PASS_DESC passDesc = {};
            pass->GetDesc(&passDesc);

            if (FAILED(m_Device->CreateInputLayout(
            elements, numElements, passDesc.pIAInputSignature,
            passDesc.IAInputSignatureSize, inputLayout.GetAddressOf())))
              return E_FAIL;

            
            desc.m_elementsDesc = elements;
            desc.m_numPasses = numElements;
            m_ObjectDesc = &desc;
            m_InputLayouts.push_back(inputLayout);
        }

        for (uint32 i = 0; i < ETOI(ConstantBuffer::END); ++i)
        {
            ID3DX11EffectConstantBuffer* constantBuffer =
                m_Effect->GetConstantBufferByName(ShaderCB::Names[i].data());

            if (nullptr != constantBuffer && constantBuffer->IsValid())
                m_ConstantBuffers[i] = constantBuffer;
        }
    } else
    return E_FAIL;

	return Component::Initialize_Prototype();
}

HRESULT Shader::Initialize_Prototype()
{
	return Component::Initialize_Prototype();
}

HRESULT Shader::Initialize(void* arg)
{
	return Component::Initialize(arg);
}

void Shader::On_Destroy() {
  m_InputLayouts.clear();

  for (uint32 i = 0; i < ETOI(ConstantBuffer::END); ++i)
  {
      m_AllocatedConstantBuffers[i].Reset();
      m_ConstantBuffers[i].Reset();
      m_AllocatedConstantBufferSizes[i] = 0;
  }

  m_Effect.Reset();

  Component::On_Destroy();
}

HRESULT Shader::Begin(uint32 passIndex) {
    if (passIndex >= m_NumPasses || nullptr == m_InputLayouts[passIndex])
      return S_OK;

    m_Effect->GetTechniqueByIndex(0)->GetPassByIndex(passIndex)->Apply(0, m_Context.Get());
    m_Context->IASetInputLayout(m_InputLayouts[passIndex].Get());

    return S_OK;
}

HRESULT Shader::Bind_SRV(const Char *constantName, const ComPtr<ID3D11ShaderResourceView> &srv) 
{
    if (!m_Effect) return S_OK;

	ID3DX11EffectVariable* variable = m_Effect->GetVariableByName(constantName);
	if (nullptr == variable) {
		MSG_BOX("Failed To Throw Value To Shader");
		return E_FAIL;
	}

	ID3DX11EffectShaderResourceVariable* srvVariable = variable->AsShaderResource();
    if (nullptr == srvVariable) {
		MSG_BOX("Shader Types Do Not Match");
		return E_FAIL;
    }

    return srvVariable->SetResource(srv.Get());
}

HRESULT Shader::Bind_Matrix(const Char *constantName, const Float4x4 *matrix) 
{
    if (!m_Effect) return S_OK;

    if (nullptr == constantName || nullptr == matrix)
        return E_INVALIDARG;

    if (0 == strcmp(constantName, WorldMatrix) && Supports_CBuffer(ConstantBuffer::Object))
    {
        ObjectCB objectBuffer{};
        objectBuffer.worldMatrix = *matrix;
        return Bind_CBufferData(objectBuffer);
    }

	ID3DX11EffectVariable* variable = m_Effect->GetVariableByName(constantName);
    if (nullptr == variable) {
		MSG_BOX("Failed To Throw Value To Shader");
		return E_FAIL;
    }

	ID3DX11EffectMatrixVariable* matrixVariable = variable->AsMatrix();
    if (nullptr == matrixVariable) {
        MSG_BOX("Matrix Types Do Not Match");
        return E_FAIL;
    }

    return matrixVariable->SetMatrix(reinterpret_cast<const Float*>(matrix));
}

HRESULT Shader::Bind_Matrices(const Char* constantName, const Float4x4* matrices, uint32 numMatrices)
{
    ID3DX11EffectVariable* variable = m_Effect->GetVariableByName(constantName);
    if (nullptr == variable)
    {
        MSG_BOX("Failed to throw value to shader");
        LOG_ERROR(L"Failed to throw value to shader {}", Helper::To_wString(constantName));
        return E_FAIL;
    }

  	ID3DX11EffectMatrixVariable* matrixVariable = variable->AsMatrix();
    if (nullptr == matrixVariable)
    {
        MSG_BOX("Shader types do not match");
        LOG_ERROR(L"Shader types do not match {}", Helper::To_wString(constantName));
        return E_FAIL;
    }

    return matrixVariable->SetMatrixArray(reinterpret_cast<const Float*>(matrices), 0, numMatrices);
}

HRESULT Shader::Bind_RawValue(const Char* constantName, const void* data, uint32 length)
{
    if (!m_Effect) return S_OK;

    ID3DX11EffectVariable* variable = m_Effect->GetVariableByName(constantName);
    if (nullptr == variable) {
        MSG_BOX("Failed To Throw Value To Shader");
        return E_FAIL;
    }

	return variable->SetRawValue(data, 0, length);
}

Bool Shader::Supports_CBuffer(ConstantBuffer type) const
{
    const uint32 index = ETOI(type);
    return index < ETOI(ConstantBuffer::END) &&
        nullptr != m_ConstantBuffers[index] && m_ConstantBuffers[index]->IsValid();
}

HRESULT Shader::Update_CBuffer(ConstantBuffer type, const void* data, size_t size)
{
    if (nullptr == data || 0 == size || 0 != size % 16)
        return E_INVALIDARG;

    if (size > D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT * 16ull)
        return E_INVALIDARG;

    const uint32 index = ETOI(type);
    if (index >= ETOI(ConstantBuffer::END))
        return E_INVALIDARG;

    if (!Supports_CBuffer(type))
        return S_OK;

    if (nullptr == m_AllocatedConstantBuffers[index] ||
        m_AllocatedConstantBufferSizes[index] != size)
    {
        m_AllocatedConstantBuffers[index].Reset();
        m_AllocatedConstantBufferSizes[index] = 0;

        D3D11_BUFFER_DESC bufferDesc{};
        bufferDesc.ByteWidth = static_cast<UINT>(size);
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        if (FAILED(m_Device->CreateBuffer(
            &bufferDesc, nullptr, m_AllocatedConstantBuffers[index].GetAddressOf())))
            return E_FAIL;

        m_AllocatedConstantBufferSizes[index] = static_cast<uint32>(size);
    }

    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (FAILED(m_Context->Map(
        m_AllocatedConstantBuffers[index].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
        return E_FAIL;

    memcpy(mapped.pData, data, size);
    m_Context->Unmap(m_AllocatedConstantBuffers[index].Get(), 0);

    return m_ConstantBuffers[index]->SetConstantBuffer(
        m_AllocatedConstantBuffers[index].Get());
}

Shared<Shader> Shader::CreatePrototype()
{
    auto shader = make_shared<Shader>(
        GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context());

    if (FAILED(shader->Initialize_Prototype())) {
        MSG_BOX("Failed To Create : Shader");
    }

    return shader;
}

Shared<Shader> Shader::Create(const ComPtr<ID3D11Device> &device,
                              const ComPtr<ID3D11DeviceContext> &context,
                              const tChar *shaderFilePath,
                              const D3D11_INPUT_ELEMENT_DESC *elements,
                              uint32 numElements) 
{
    auto shader = make_shared<Shader>(device, context);
    if (FAILED(shader->Initialize_Prototype(shaderFilePath, elements, numElements))) {
      MSG_BOX("Failed To Create : Shader");
    }

    return shader;
}

Shared<Component> Shader::Clone(void* arg) {
    if (arg == nullptr)
    {
        LOG_ERROR(L"There is no ShaderDesc");
        MSG_BOX("There is no ShaderDesc");
        return nullptr;
    }

    SHADER_DESC& desc = *static_cast<SHADER_DESC*>(arg);
    auto resShader = GAME_INSTANCE->Get_Shader(0, desc.m_VertexTag.c_str());

	auto shader = make_shared<Shader>(*resShader.get());

    if (FAILED(shader->Initialize(arg))) {
    	MSG_BOX("Failed To Cloned : Shader");
    }
    return shader;
}
