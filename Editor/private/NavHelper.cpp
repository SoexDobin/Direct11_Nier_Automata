#include "pch.h"
#include "NavHelper.h"

NavHelper::NavHelper() : EditorObject()
{
}

NavHelper::~NavHelper()
{
}

HRESULT NavHelper::Initialize()
{
	return EditorObject::Initialize();
}

void NavHelper::Update(Bool isResize)
{
	EditorObject::Update(isResize);
}

void NavHelper::Render(Bool isResize)
{
	EditorObject::Render(isResize);
}

void NavHelper::BakePreview()
{
}

void NavHelper::SaveBinary()
{
}
