#include "Shader.h"
#include "Game.h"
#include "SpdLogger.h"

Shader::Shader() : Component{} {}
Shader::Shader(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
    : Component(device, context) {}

Shader::Shader(const Shader& rhs)
    : Component(rhs), m_Effect{rhs.m_Effect}, m_NumPasses{rhs.m_NumPasses},
      m_InputLayouts{rhs.m_InputLayouts} {}

HRESULT Shader::Initialize_Prototype(const tChar* shaderFilePath, const D3D11_INPUT_ELEMENT_DESC* elements, uint32 numElements) 
{
    uint32 hlslFlag = {};

#ifdef _DEBUG
        hlslFlag |= D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
#else
		hlslFlag |= D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

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
