#pragma once
#include "PartObject.h"
#include "Shader.h"

NS_BEGIN(Engine)
class Model;
NS_END

NS_BEGIN(Client)

class Em3000;

class CLIENT_DLL Em3000Parts abstract : public PartObject
{
	RTTR_ENABLE(PartObject)
public:
	typedef struct tagEm3000PartsDesc : public PARTOBJECT_DESC
	{
		Shader::SHADER_DESC shaderDesc{};
		wstring modelResourceTag{};
		Weak<Model> bodyModel{};
		const Char* targetBoneName{ nullptr };
	} EM3000PART_DESC;
public:
	explicit Em3000Parts();
	explicit Em3000Parts(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context); 
	explicit Em3000Parts(const Em3000Parts& rhs);
	virtual ~Em3000Parts() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void Set_Pl0000Container(const Shared<Em3000>& em3000) { m_Em3000 = em3000; }

public:
	Shared<Model> Get_ModelComponent() { return m_Model; }
	virtual void Set_Animation(uint32 animIndex, Float blendDuration, Bool isLoop);
	uint32 Get_CurrentAnimationIndex() const;
	uint32 Get_NextAnimationIndex() const; 
	Float Get_AnimationProgress() const; 
	Bool Is_AnimationFinished() const; 

public:
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	
	HRESULT Render() override;
	void Submit_RenderGroup() override;

protected:
	virtual HRESULT Bind_ShaderResources();

private:
	HRESULT Ready_Components(Shader::SHADER_DESC shaderDesc, const wstring& modelResourceTag);

protected:
	Shared<Shader>	m_Shader{ nullptr };
	Shared<Model>	m_Model{ nullptr };
	Weak<Em3000>	m_Em3000{};

protected:
	Weak<Model>		m_BodyModel{};
	string			m_BoneName{};
	int32			m_TargetBoneIndex{ -1 };

public:
	Shared<GameObject> Clone(void* arg) override PURE;
};

NS_END