#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)
NS_BEGIN(Helper)

#define TYPE_OBJECT(CLASSTYPE) *CLASSTYPE

template <typename T>
static rttr::type Get_Type()
{
    rttr::type typeInfo = rttr::type::get<T>();

    if (!typeInfo.is_valid())
    {
        MSG_BOX("Failed To Get Type");
        return rttr::type::get<void>();
    }

    return typeInfo;
}
template <typename T>
static rttr::type Get_Type(T* p)
{
    rttr::type typeInfo = rttr::type::get(TYPE_OBJECT(p));

    if (!typeInfo.is_valid())
    {
        MSG_BOX("Failed To Get Type");
        return rttr::type::get<void>();
    }

    return typeInfo;
}

static const array_range<rttr::type>& Get_Types()
{
    return rttr::type::get_types();
}

NS_END
NS_END

