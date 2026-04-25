#pragma once
#include "StateEm3000.h"

NS_BEGIN(Client)

class CLIENT_DLL StateEm3000_Melee final : public StateEm3000
{
public:
	explicit StateEm3000_Melee(const wstring& tag, const Shared<Em3000>& owner);
	~StateEm3000_Melee() override = default;

private:
	HRESULT Initialize() override;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

private:
	Vector3 m_OriginalPosition{};      
	Float m_MixerTime{ 0.f };     
	Float m_MixerDuration{ 5.f }; 

public:
	static Shared<StateEm3000_Melee> Create(const wstring& tag, const Shared<Em3000>& owner);
};



NS_END
