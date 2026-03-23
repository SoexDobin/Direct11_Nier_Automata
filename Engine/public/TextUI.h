#pragma once
#include "UIObject.h"


NS_BEGIN(Engine)

class ENGINE_DLL TextUI : public UIObject
{
	RTTR_ENABLE(UIObject)
public:
	typedef struct tagTextUIDesc : public UI_DESC
	{
		wstring fontTag;
		const tChar* text;
	} UI_TEXT_DESC;
public:
	explicit TextUI();
	explicit TextUI(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit TextUI(const TextUI& rhs);
	virtual ~TextUI() override = default;
	
public:
	const wstring& Get_FontTag() const { return m_fontTag; }
	void Set_FontTag(const wstring& newFontTag) { m_fontTag = newFontTag; }

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	
public:
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void Fixed_Update(Float fixedDelta) override;
	HRESULT Render() override;
	void Submit_RenderGroup() override;

protected:
	wstring m_fontTag{};
	const tChar* m_Text{nullptr};

public:
	static Shared<TextUI> CreatePrototype();
	static Shared<TextUI> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg = nullptr) override;
};

NS_END