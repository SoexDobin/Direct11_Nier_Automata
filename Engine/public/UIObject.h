#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class Shader;

class ENGINE_DLL UIObject abstract : public GameObject
{
public:
	typedef struct tagUIObjectDesc
	{
		Float x, y, sizeX, sizeY;
	} UI_DESC;

public:
	UIObject();
	UIObject(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	UIObject(const Shared<UIObject>& rhs);
	virtual ~UIObject() override = default; 

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* arg) override;
	virtual void On_Destroy() override;
	virtual void On_Enable() override;
	virtual void On_Disable() override;
	virtual void Set_Active(Bool isActive) override;

public:
	virtual void Priority_Update(Float timeDelta) override;
	virtual void Update(Float timeDelta) override;
	virtual void Late_Update(Float timeDelta) override;
	virtual void Fixed_Update(Float fixedDelta) override;
	virtual HRESULT Render() override;
	
protected:
	void Update_Transform() const;
	HRESULT Bind_ShaderResource(const Shared<Shader>& shader, const Char* constantName, D3DTS transformState) const;

private:
	Float		m_X{}, m_Y{}, m_SizeX{}, m_SizeY{};
	Float		m_ViewportWidth{}, m_ViewportHeight{};
	Matrix		m_TransformationMatrices[ETOI(D3DTS::END)];

public:
	Shared<GameObject> Clone(void* arg) override PURE;
};

NS_END