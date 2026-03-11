#include "UIObject.h"

#include "Game.h"
#include "Shader.h"

UIObject::UIObject() : GameObject() {}

UIObject::UIObject(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context)
    : GameObject{device, context} {}

UIObject::UIObject(const UIObject& rhs)
    : GameObject{rhs}, m_TransformationMatrices{} 
{
    m_TransformationMatrices[ETOI(D3DTS::VIEW)] = rhs.m_TransformationMatrices[ETOI(D3DTS::VIEW)];
    m_TransformationMatrices[ETOI(D3DTS::PROJ)] = rhs.m_TransformationMatrices[ETOI(D3DTS::PROJ)];
}

HRESULT UIObject::Initialize_Prototype() {
  return GameObject::Initialize_Prototype();
}

HRESULT UIObject::Initialize(void *arg) {
    if (arg != nullptr) {
        UI_DESC *desc = static_cast<UI_DESC *>(arg);

        m_X = desc->x;
        m_Y = desc->y;
        m_SizeX = desc->sizeX;
        m_SizeY = desc->sizeY;
    }

	if (FAILED(GameObject::Initialize(arg)))
		return E_FAIL;

    D3D11_VIEWPORT viewPortDesc = GAME_INSTANCE->Get_ViewportDesc();
    uint32 numViewPorts{1};
    m_ViewportWidth = viewPortDesc.Width;
    m_ViewportHeight = viewPortDesc.Height;

    Update_UITransform();

    XMStoreFloat4x4(&m_TransformationMatrices[ETOI(D3DTS::VIEW)], Matrix::Identity);
    XMStoreFloat4x4(&m_TransformationMatrices[ETOI(D3DTS::PROJ)],
        XMMatrixOrthographicLH(m_ViewportWidth, m_ViewportHeight, 0.f, 1.f));

    return S_OK;
}

void UIObject::On_Destroy()                 { GameObject::On_Destroy(); }
void UIObject::On_Enable()                  { GameObject::On_Enable(); }
void UIObject::On_Disable()                 { GameObject::On_Disable(); }
void UIObject::Set_Active(Bool isActive)    { GameObject::Set_Active(isActive); }
void UIObject::Priority_Update(Float timeDelta) {
	GameObject::Priority_Update(timeDelta);
}
void UIObject::Update(Float timeDelta) {
	GameObject::Update(timeDelta);
}
void UIObject::Late_Update(Float timeDelta) {
	GameObject::Late_Update(timeDelta);
}
void UIObject::Fixed_Update(Float fixedDelta) {
	GameObject::Fixed_Update(fixedDelta);
}
HRESULT UIObject::Render() {
	return GameObject::Render();
}

void UIObject::Update_UITransform() const {
    Float baseWidth = m_ViewportWidth;
    Float baseHeight = m_ViewportHeight;
    auto parent = Get_Parent();
    if (parent) {
        // 부모가 UIObject라면 그 크기를 가져옴 (캐스팅 필요)
        auto uiParent = std::dynamic_pointer_cast<UIObject>(parent);
        if (uiParent) {
            baseWidth = uiParent->Get_SizeX();
            baseHeight = uiParent->Get_SizeY();
        }
    }

    Float anchorRatioX = 0.5f;
    Float anchorRatioY = 0.5f;

    switch (m_Anchor) {
    case UI_ANCHOR::TOP_LEFT:      anchorRatioX = 0.0f; anchorRatioY = 0.0f; break;
    case UI_ANCHOR::TOP_CENTER:    anchorRatioX = 0.5f; anchorRatioY = 0.0f; break;
    case UI_ANCHOR::TOP_RIGHT:     anchorRatioX = 1.0f; anchorRatioY = 0.0f; break;
    case UI_ANCHOR::CENTER_LEFT:   anchorRatioX = 0.0f; anchorRatioY = 0.5f; break;
    case UI_ANCHOR::CENTER:        anchorRatioX = 0.5f; anchorRatioY = 0.5f; break;
    case UI_ANCHOR::CENTER_RIGHT:  anchorRatioX = 1.0f; anchorRatioY = 0.5f; break;
    case UI_ANCHOR::BOTTOM_LEFT:   anchorRatioX = 0.0f; anchorRatioY = 1.0f; break;
    case UI_ANCHOR::BOTTOM_CENTER: anchorRatioX = 0.5f; anchorRatioY = 1.0f; break;
    case UI_ANCHOR::BOTTOM_RIGHT:  anchorRatioX = 1.0f; anchorRatioY = 1.0f; break;
    }

    Float anchorPosX = baseWidth * anchorRatioX;
    Float anchorPosY = baseHeight * anchorRatioY;

    m_Transform->Set_Scale(m_SizeX, m_SizeY, 1.f);
    m_Transform->Set_Position(Vector3{
    	(m_X + anchorPosX) - (baseWidth * 0.5f),
    	-(m_Y + anchorPosY) + (baseHeight * 0.5f),
    	0.f});

    m_Transform->Update_WorldMatrix();
}

HRESULT UIObject::Bind_ShaderResource(const Shared<Shader> &shader,
                                      const Char *constantName,
                                      D3DTS transformState) const {
	return shader->Bind_Matrix(constantName, &m_TransformationMatrices[ETOI(transformState)]);
}
