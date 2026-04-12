#include "Font.h"

Font::Font() {}
Font::Font(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) 
	: m_Device{ device }, m_Context{context} {}
Font::Font(const Font& rhs) {}

HRESULT Font::Initialize_Prototype(const tChar* fontFilePath)
{
	m_Batch = make_shared<SpriteBatch>(m_Context.Get());
	m_Font = make_shared<SpriteFont>(m_Device.Get(), fontFilePath);

	return S_OK;
}

HRESULT Font::Initialize(void* arg) { return Object::Initialize(arg); }

void Font::Draw(const tChar* text, const Vector2& position, const Color& color)
{
	m_Context->GSSetShader(nullptr, nullptr, 0);

	m_Batch->Begin();

	m_Font->DrawString(m_Batch.get(), text, position, color);

	m_Batch->End();
}

Shared<Font> Font::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const tChar* fontFilePath)
{
	auto instance = make_shared<Font>(device, context);

	if (FAILED(instance->Initialize_Prototype(fontFilePath)))
	{
		MSG_BOX("Failed to Created : Font");
		return nullptr;
	}

	return instance;
}
