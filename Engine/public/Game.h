#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class GraphicDevice;
class TimeManager;
class LevelManager;
class PrototypeManager;
class ObjectManager;
class Renderer;

class ENGINE_DLL Game
{
	DECLARE_SINGLETON(Game)

protected:
	explicit Game() = default;
	~Game() = default;
public:
	Get_TimeManager()->;
public:
	HRESULT Initialize_Engine(const ENGINE_DESC& engineDesc,
							  _Out_ ComPtr<ID3D11Device>& device,
							  _Out_ ComPtr<ID3D11DeviceContext>& context);
	void	Update_Engine();
	HRESULT Draw();
	void	Clear_Resource(uint32 levIndex);

public: /* For GraphicDevice */
	HRESULT Clear_BackBufferView(const Shared<Float4>& clearColor) const;
	HRESULT Present() const;

public: /* For TimeManager */
	HRESULT Add_Timer(const wstring& timerTag) const;
	Float	Compute_TimeDelta(const wstring& timerTag) const;

public: /* For LevelManager */
	HRESULT Change_Level(uint32 levIndex, Unique<class Level> newLevel);

public: /* For PrototypeManager */
	HRESULT Add_Prototype(uint32 levIndex, const Shared<class Object>& prototype) const;
	
public: /* For ObjectManager */
	HRESULT Add_GameObject(Shared<void> arg = nullptr) const;

public: /* For Renderer */
	void Add_RenderGroup(RENDERGROUP group, const Shared<class GameObject>& gameObject) const;

public: /* Util */
	template<typename T>
	Shared<GameObject> Get_Prototype() const;
	Shared<GameObject> Get_Prototype(uint32 typeID) const;
	Shared<GameObject> Get_Prototype(const wstring& className) const;
	template<typename T>
	Shared<GameObject> Get_GameObject() const;
	Shared<GameObject> Get_GameObject(uint32 uniqueID) const;
	Shared<GameObject> Get_GameObject(const wstring& className) const;

private:
	Unique<GraphicDevice>		m_GraphicDevice = { nullptr };
	Unique<TimeManager>			m_TimeManager = { nullptr };
	Unique<LevelManager>		m_LevelManager = { nullptr };
	Unique<PrototypeManager>	m_PrototypeManager = { nullptr };
	Unique<ObjectManager>		m_ObjectManager = { nullptr };
	Unique<Renderer>			m_Renderer = { nullptr };
};

NS_END
