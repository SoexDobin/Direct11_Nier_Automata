#include "LoadingBackground.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "pch.h"

#include "SpdLogger.h"

LoadingBackground::LoadingBackground() : UIObject() {}

LoadingBackground::LoadingBackground(const ComPtr<ID3D11Device> &device,
                                     const ComPtr<ID3D11DeviceContext> &context)
    : UIObject{device, context} {}

LoadingBackground::LoadingBackground(const LoadingBackground& rhs)
    : UIObject{rhs} {}

HRESULT LoadingBackground::Initialize_Prototype() {

  return UIObject::Initialize_Prototype();
}

HRESULT LoadingBackground::Initialize(void *arg) {

    LOADING_BG_UI desc{};

    desc.x = 100.f;
    desc.y = 100.f;
    desc.sizeX = 200.f;
    desc.sizeY = 200.f;

	return UIObject::Initialize(&desc);
}

void LoadingBackground::On_Destroy() { UIObject::On_Destroy(); }

void LoadingBackground::Priority_Update(Float timeDelta) {
  UIObject::Priority_Update(timeDelta);
}

void LoadingBackground::Update(Float timeDelta) {
	UIObject::Update(timeDelta);
}

void LoadingBackground::Late_Update(Float timeDelta) {
  UIObject::Late_Update(timeDelta);
}

void LoadingBackground::Fixed_Update(Float fixedDelta) {
    UIObject::Fixed_Update(fixedDelta);
    GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::UI, shared_from_this());
}

HRESULT LoadingBackground::Render() {
  

  return UIObject::Render();
}

HRESULT LoadingBackground::Ready_Components()
{
    return S_OK;
}

Shared<LoadingBackground> LoadingBackground::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) {
  auto bg = make_shared<LoadingBackground>(device, context);

  if (FAILED(bg->Initialize_Prototype())) {
    LOG_ERROR(L"Failed To Create LoadingBackground");
    return nullptr;
  }

  return bg;
}

Shared<GameObject> LoadingBackground::Clone(void *arg) {
  auto bg = make_shared<LoadingBackground>(*this);

  if (FAILED(bg->Initialize(arg))) {
    LOG_ERROR(L"Failed To CreateComponent LoadingBackground");
    return nullptr;
  }

  return bg;
}
