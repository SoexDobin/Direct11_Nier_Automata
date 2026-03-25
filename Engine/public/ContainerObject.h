#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class PartObject;

class ENGINE_DLL ContainerObject abstract : public GameObject
{
	RTTR_ENABLE(GameObject)
public:
	typedef struct tagContainerObjectDesc : public GameObject {} CONTAINEROBJECT_DESC;
public:
	explicit ContainerObject();	
	explicit ContainerObject(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& rhs);
	explicit ContainerObject(const ContainerObject& rhs);
	~ContainerObject() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;

public:
	GAMEOBJECTTYPE Get_GameObjectType() final { return GAMEOBJECTTYPE::CONTAINER; }
	void Priority_Update(Float timeDelta) override {}
	void Update(Float timeDelta) override {}
	void Late_Update(Float timeDelta) override {}
	void Fixed_Update(Float fixedDelta) override {}
	HRESULT Render() override { return S_OK; }
	virtual void Submit_RenderGroup() override {}

public:
	HRESULT Add_PartObject(uint32 prototypeLevIndex, const wstring& prototypeTag, const wstring& partTag, void* arg = nullptr); 
	Shared<PartObject> Find_PartObject(const wstring& partTag);

protected:
	unordered_map<wstring, Shared<PartObject>> m_PartObjects;

public:
	virtual Shared<GameObject> Clone(void* arg) PURE;

};

NS_END