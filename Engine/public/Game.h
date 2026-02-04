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
	~Game() = default;

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
	HRESULT Add_GameObject(const Shared<void>& arg = nullptr) const;

public: /* For Renderer */
	void Add_RenderGroup(RENDERGROUP group, const Shared<class GameObject>& gameObject) const;

public: /* Util */
	template<typename T>
	constexpr Shared<const T> Find_Prototype(uint32 levIndex = MAXINT32) const;
	inline Shared<const GameObject> Find_Prototype(uint32 typeID, uint32 levIndex = MAXINT32) const;
	inline Shared<const GameObject> Find_Prototype(const wstring& className, uint32 levIndex = MAXINT32) const;
	
	template<typename T>
	constexpr Shared<T> Instantiated(const Shared<void>& arg) const;
	inline Shared<GameObject> Instantiated(uint32 typeID, const Shared<void>& arg) const;
	inline Shared<GameObject> Instantiated(const wstring& className, const Shared<void>& arg) const;

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

layer, tag를 툴과 클라이언트 연동을 위한 Regist 클래스
Reflection 을 통해 자동으로 Client의 스크립트를 읽어오는 rttr을
감싸는 wrapper 클래스는 어떻게 구성하는게 좋고 사용엔진 처럼 엔진은 무조건 적으로 독립적이어야해
Engine의 모든 헤더를 읽고 판단해줘 필요하다면 cpp나 다른 파일도