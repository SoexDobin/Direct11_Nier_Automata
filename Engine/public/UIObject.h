#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class Shader;
class Texture;
class VIBuffer_Rect;

class ENGINE_DLL UIObject abstract : public GameObject {
public:
  typedef struct tagUIObjectDesc : public GAMEOBJECT_DESC {
    UI_ANCHOR anchor = { UI_ANCHOR::CENTER };
    Float x{}, y{}, sizeX{}, sizeY{};
  } UI_DESC;

public:
  UIObject();
  UIObject(const ComPtr<ID3D11Device> &device,
           const ComPtr<ID3D11DeviceContext> &context);
  UIObject(const UIObject& rhs);
  virtual ~UIObject() override = default;

public:
    void Set_Anchor(UI_ANCHOR anchor) { m_Anchor = anchor; Update_UITransform(); }
    void Set_UIPosition(Vector2 position) { m_X = position.x; m_Y = position.y; Update_UITransform(); }

public:
    void Set_Active(Bool isActive) final;
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void *arg) override;
    virtual void On_Destroy() override;
    virtual void On_Enable() override;
    virtual void On_Disable() override;


public:
    virtual void Priority_Update(Float timeDelta) override;
    virtual void Update(Float timeDelta) override;
    virtual void Late_Update(Float timeDelta) override;
    virtual void Fixed_Update(Float fixedDelta) override;
    virtual HRESULT Render() override;

protected:
    void Update_UITransform() const;
    HRESULT Bind_ShaderResource(const Shared<Shader> &shader,
                              const Char *constantName,
                              D3DTS transformState) const;

protected:
    UI_ANCHOR m_Anchor = { UI_ANCHOR::CENTER };
    Float m_X{}, m_Y{}, m_SizeX{}, m_SizeY{};
    Float m_ViewportHeight{}, m_ViewportWidth{};
    Matrix m_TransformationMatrices[ETOI(D3DTS::END)];

public:
	virtual Shared<GameObject> Clone(void *arg = nullptr) override PURE;
};

NS_END