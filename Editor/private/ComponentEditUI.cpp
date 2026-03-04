#include "pch.h"
#include "ComponentEditUI.h"

#include <SpdLogger.h>

ComponentEditUI::ComponentEditUI()
{
}

HRESULT ComponentEditUI::Initialize()
{

	return S_OK;
}

void ComponentEditUI::Update()
{
	
}

void ComponentEditUI::Render()
{
	
}

Shared<ComponentEditUI> ComponentEditUI::Create()
{
	auto editUI = make_shared<ComponentEditUI>();

	if (FAILED(editUI->Initialize()))
	{
		LOG_ERROR("Failed to Create Inspector Component GUI");
		return nullptr;
	}

	return editUI;
}
