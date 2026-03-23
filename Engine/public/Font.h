#pragma once
#include "Object.h"

NS_BEGIN(Engine)

class Font final : public Object
{
public:
	explicit Font();
	explicit Font(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Font(const Font& rhs);
	~Font() override = default;

public:
	HRESULT Initialize_Prototype(const tChar* fontFilePath);
	HRESULT Initialize(void* arg) override;
	PROTOTYPE Get_Prototype() const override { return PROTOTYPE::FONT; }

public:
	void Draw(const tChar* text, const Vector2& position, const Color& color);

private:
	ComPtr<ID3D11Device> m_Device{ nullptr };
	ComPtr<ID3D11DeviceContext> m_Context{ nullptr };

	Shared<SpriteBatch> m_Batch{ nullptr };
	Shared<SpriteFont> m_Font{ nullptr };

public:
	static Shared<Font> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const tChar* fontFilePath);
	
};

NS_END