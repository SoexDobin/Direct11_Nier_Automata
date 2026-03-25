#include "TextUI.h"

#include "Game.h"
#include "Renderer.h"

TextUI::TextUI()
{
}

TextUI::TextUI(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: UIObject{device, context} {}

TextUI::TextUI(const TextUI& rhs)
	: UIObject{rhs} {}

HRESULT TextUI::Initialize_Prototype()
{
	return UIObject::Initialize_Prototype();
}

HRESULT TextUI::Initialize(void* arg)
{
	return UIObject::Initialize(arg);
}

void TextUI::Priority_Update(Float timeDelta)
{
	UIObject::Priority_Update(timeDelta);
}

void TextUI::Update(Float timeDelta)
{
	UIObject::Update(timeDelta);
}

void TextUI::Late_Update(Float timeDelta)
{
	UIObject::Late_Update(timeDelta);
}

void TextUI::Fixed_Update(Float fixedDelta)
{
	UIObject::Fixed_Update(fixedDelta);
}

HRESULT TextUI::Render()
{
	if (m_fontTag.empty()) return S_OK;
	if (!Is_Active()) return S_OK;

	Vector2 uiPos = Vector2{ m_Transform->Get_LocalPosition().x, m_Transform->Get_LocalPosition().y };
	GAME_INSTANCE->Draw_Font(m_fontTag, m_Text, uiPos);

	return S_OK;
}

void TextUI::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::UI, shared_from_this());
}

Shared<TextUI> TextUI::CreatePrototype()
{
	return Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context());
}

Shared<TextUI> TextUI::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<TextUI>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TextUI");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> TextUI::Clone(void* arg)
{
	auto instance = make_shared<TextUI>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : TextUI");
		return nullptr;
	}

	return instance;
}


