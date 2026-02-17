#include "Shader.h"

Shader::Shader(const ComPtr<ID3D11Device> &device,
               const ComPtr<ID3D11DeviceContext> &context)
    : Component(device, context) {}

Shader::Shader(const Shared<Shader> &rhs)
    : Component(rhs), m_Effect{rhs->m_Effect}, m_NumPasses{rhs->m_NumPasses},
      m_InputLayouts{rhs->m_InputLayouts} {}

HRESULT Shader::Initialize_Prototype(const tChar *shaderFilePath,
                                     const D3D11_INPUT_ELEMENT_DESC *elements,
                                     uint32 numElements) {
  uint32 hlslFlag = {};

  if constexpr (_DEBUG)
    hlslFlag |= D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
  else
    hlslFlag |= D3DCOMPILE_OPTIMIZATION_LEVEL1;

  if (FAILED(D3DX11CompileEffectFromFile(
          shaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, hlslFlag,
          0, m_Device.Get(), m_Effect.GetAddressOf(), nullptr)))
    return E_FAIL;

  if (ComPtr<ID3DX11EffectTechnique> technique =
          m_Effect->GetTechniqueByIndex(0)) {
    D3DX11_TECHNIQUE_DESC techniqueDesc = {};

    technique->GetDesc(&techniqueDesc);
    m_NumPasses = techniqueDesc.Passes;
    m_InputLayouts.reserve(m_NumPasses);

    for (uint32 i = 0; i < m_NumPasses; ++i) {
      ComPtr<ID3D11InputLayout> inputLayout = {nullptr};
      ComPtr<ID3DX11EffectPass> pass = technique->GetPassByIndex(i);
      if (nullptr == pass)
        return E_FAIL;

      D3DX11_PASS_DESC passDesc = {};
      pass->GetDesc(&passDesc);

      if (FAILED(m_Device->CreateInputLayout(
              elements, numElements, passDesc.pIAInputSignature,
              passDesc.IAInputSignatureSize, inputLayout.GetAddressOf())))
        return E_FAIL;

      m_InputLayouts.push_back(inputLayout);
    }
  } else
    return E_FAIL;

  return Component::Initialize_Prototype();
}

HRESULT Shader::Initialize(void *arg) { return Component::Initialize(arg); }

void Shader::On_Destroy() {
  m_InputLayouts.clear();

  Component::On_Destroy();
}

HRESULT Shader::Begin(uint32 passIndex) {
  if (passIndex >= m_NumPasses || nullptr == m_InputLayouts[passIndex])
    return E_FAIL;

  m_Effect->GetTechniqueByIndex(0)->GetPassByIndex(passIndex)->Apply(
      0, m_Context.Get());
  m_Context->IASetInputLayout(m_InputLayouts[passIndex].Get());

  return S_OK;
}

HRESULT Shader::Bind_SRV(const Char *constantName,
                         const ComPtr<ID3D11ShaderResourceView> &srv) {
  ComPtr<ID3DX11EffectVariable> variable =
      m_Effect->GetVariableByName(constantName);
  if (nullptr == variable) {
    MSG_BOX("Failed To Throw Value To Shader");
    return E_FAIL;
  }

  ComPtr<ID3DX11EffectShaderResourceVariable> srvVariable =
      variable->AsShaderResource();
  if (nullptr == srvVariable) {
    MSG_BOX("Shader Types Do Not Match");
    return E_FAIL;
  }

  return srvVariable->SetResource(srv.Get());
}

HRESULT Shader::Bind_Matrix(const Char *constantName, const Float4x4 *matrix) 
{
	ComPtr<ID3DX11EffectVariable> variable = m_Effect->GetVariableByName(constantName);
    if (nullptr == variable) {
		MSG_BOX("Failed To Throw Value To Shader");
		return E_FAIL;
    }

  ComPtr<ID3DX11EffectMatrixVariable> matrixVariable = variable->AsMatrix();
    if (nullptr == matrixVariable) {
        MSG_BOX("Matrix Types Do Not Match");
        return E_FAIL;
    }

    return matrixVariable->SetMatrix(reinterpret_cast<const Float*>(matrix));
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

Shared<Component> Shader::Clone(void *arg) {
	auto shader = make_shared<Shader>(shared_from_this());

    if (FAILED(shader->Initialize(arg))) {
    	MSG_BOX("Failed To Cloned : Shader");
    }
    return shader;
}
