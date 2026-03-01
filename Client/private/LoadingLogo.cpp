#include "LoadingLogo.h"
#include "pch.h"


#include "SpdLogger.h"

LoadingLogo::LoadingLogo() : UIObject() {}

LoadingLogo::LoadingLogo(const ComPtr<ID3D11Device> &device,
                         const ComPtr<ID3D11DeviceContext> &context)
    : UIObject(device, context) {}

LoadingLogo::LoadingLogo(const LoadingLogo& rhs) : UIObject(rhs) {}

HRESULT LoadingLogo::Initialize_Prototype() {
  return UIObject::Initialize_Prototype();
}

HRESULT LoadingLogo::Initialize(void *arg) { return UIObject::Initialize(arg); }

void LoadingLogo::On_Destroy() { UIObject::On_Destroy(); }

void LoadingLogo::Priority_Update(Float timeDelta) {
  UIObject::Priority_Update(timeDelta);
}

void LoadingLogo::Update(Float timeDelta) { UIObject::Update(timeDelta); }

void LoadingLogo::Late_Update(Float timeDelta) {
  UIObject::Late_Update(timeDelta);
}

void LoadingLogo::Fixed_Update(Float fixedDelta) {
  UIObject::Fixed_Update(fixedDelta);
}

HRESULT LoadingLogo::Render() {
  //if (nullptr == m_Shader || nullptr == m_Texture || nullptr == m_BufferRect) {
  //  return E_FAIL;
  //}

  return UIObject::Render();
}

Shared<LoadingLogo>
LoadingLogo::Create(const ComPtr<ID3D11Device> &device,
                    const ComPtr<ID3D11DeviceContext> &context) {
  auto logo = make_shared<LoadingLogo>(device, context);

  if (FAILED(logo->Initialize_Prototype())) {
    LOG_ERROR(L"Failed To Create LoadingLogo");
    return nullptr;
  }

  return logo;
}

Shared<GameObject> LoadingLogo::Clone(void *arg) {
  auto logo = make_shared<LoadingLogo>(*this);

  if (FAILED(logo->Initialize(arg))) {
    LOG_ERROR(L"Failed To Clone LoadingLogo");
    return nullptr;
  }

  return logo;
}
