#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)

class Transform;
class ENGINE_DLL WorldUIObject abstract : public UIObject {
    RTTR_ENABLE(UIObject)
public:
    typedef struct tagWorldUIObjectDesc : public UI_DESC
    {
	    
    } WORLD_UI_DESC;

public:
    explicit WorldUIObject();
    explicit WorldUIObject(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    explicit WorldUIObject(const WorldUIObject& rhs);
    virtual ~WorldUIObject() override = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* arg) override;
};

NS_END