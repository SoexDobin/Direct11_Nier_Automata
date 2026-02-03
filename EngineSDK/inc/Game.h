#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class GraphicDevice;
class TimeManager;
class PrototypeManager;
class ObjectManager;
class Renderer;

class ENGINE_DLL Game
{
	DECLARE_SINGLETON(Game)

protected:
	explicit Game();
	~Game();

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& engineDesc,
							  _Out_ ComPtr<ID3D11Device>& device,
							  _Out_ ComPtr<ID3D11DeviceContext>& context);
	void	Update_Engine();
	HRESULT Draw();
	void	Clear_Resource(uint32 levIndex);

public: /* For GraphicDevice */
	HRESULT Clear_BackBufferView(const Float4* clearColor) const;
	HRESULT Present(uint32 levIndex) const;

public: /* For TimeManager */
	Float Update_Time();

public: /* For LevelManager */
	HRESULT Change_Level(uint32 levIndex, Unique<class Level> newLevel);

public: /* For PrototypeManager */
	HRESULT Add_Prototype(uint32 levIndex, Shared<Shared<class Object>> prototype);
	template <typename T>
	Shared<T> Clone_Prototype(PROTOTYPE ePrototype, uint32 levIndex, Shared<void> arg = nullptr);
	template <typename T>
	Shared<T> Clone_Prototype(PROTOTYPE ePrototype, uint32 levIndex, uint32 typeID, Shared<void> arg = nullptr);
	template <typename T>
	Shared<T> Clone_Prototype(PROTOTYPE ePrototype, uint32 levIndex, const wstring& typeTag, Shared<void> arg = nullptr);
public: /* For ObjectManager */
	HRESULT Add_GameObject(uint32 prototypeLevIndex, const wstring& prototypeTag, uint32 layerLevIndex, const wstring& strLayerTag, Shared<void> arg = nullptr);

public: /* For Renderer */

private:
	Unique<GraphicDevice>		m_GraphicDevice = { nullptr };
	Unique<TimeManager>			m_TimeManager = { nullptr };
	Unique<PrototypeManager>	m_PrototypeManager = { nullptr };
};

NS_END
