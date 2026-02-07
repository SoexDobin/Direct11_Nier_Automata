#pragma once
#include "Engine_Define.h"

#include "GraphicDevice.h"
#include "TimeManager.h"
#include "LevelManager.h"
#include "PrototypeManager.h"
#include "ObjectManager.h"
#include "Renderer.h"

NS_BEGIN(Engine)

class ENGINE_DLL Game
{
	DECLARE_SINGLETON(Game)

protected:
	explicit Game() = default;
	~Game();

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
	uint32	Get_CurrentLevelIndex() const { return m_LevelManager->Get_CurrentLevelIndex(); }
	HRESULT Change_Level(uint32 levIndex, Unique<class Level> newLevel);

public: /* For PrototypeManager */
	uint32			Get_PrototypeID(const wstring& name) const { return m_PrototypeManager->Get_TypeByName(name); }
	const wstring&	Get_PrototypeName(uint32 typeID) const { return m_PrototypeManager->Get_NameByType(typeID); }
	HRESULT			Add_Prototype(uint32 levIndex, const Shared<class Object>& prototype) const;
	
public: /* For ObjectManager */
private:
	HRESULT Add_GameObject(const Shared<GameObject>& GameObject) const;

public: /* For Renderer */
	void Add_RenderGroup(RENDERGROUP group, const Shared<class GameObject>& gameObject) const;

public: /* Util At GameUtil.cpp*/
	template<typename T> /* Find Read only Prototype */
	constexpr Shared<const T> Find_Prototype(PROTOTYPE prototype, uint32 levIndex = MAXINT32) const;
	inline Shared<const Object> Find_Prototype(PROTOTYPE prototype, uint32 typeID, uint32 levIndex = MAXINT32) const;
	inline Shared<const Object> Find_Prototype(PROTOTYPE prototype, const wstring& className, uint32 levIndex = MAXINT32) const;
	
	template<typename T> /* Instance Object */
	constexpr Shared<T> Instantiate(const Shared<void>& arg = nullptr) const;
	template<typename T>
	constexpr Shared<T> Instantiate(uint32 typeID, const Shared<void>& arg = nullptr) const;
	template<typename T>
	constexpr Shared<T> Instantiate(const wstring& className, const Shared<void>& arg = nullptr) const;

private:
	inline Shared<Object> Instantiate_Internal(PROTOTYPE protoType, uint32 levIndex, uint32 typeID, const Shared<void>& arg = nullptr) const;
	inline Shared<Object> Instantiate_Internal(PROTOTYPE protoType, uint32 levIndex, const wstring& className, const Shared<void>& arg = nullptr) const;

	/*
	현재 레벨의 프로토타입 확인 : const Shared
	특정 레벨의 프로토 타입 확인 : const Shared
	*/

private:
	Unique<GraphicDevice>		m_GraphicDevice = { nullptr };
	Unique<TimeManager>			m_TimeManager = { nullptr };
	Unique<LevelManager>		m_LevelManager = { nullptr };
	Unique<PrototypeManager>	m_PrototypeManager = { nullptr };
	Unique<ObjectManager>		m_ObjectManager = { nullptr };
	Unique<Renderer>			m_Renderer = { nullptr };
};

NS_END
