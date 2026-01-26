#ifndef Engine_Function_h__
#define Engine_Function_h__

#include "Engine_Typedef.h"

namespace Engine
{
	template<typename T>
	void	Safe_Delete(T& ptr)
	{
		if (nullptr != ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
	}

	template<typename T>
	void	Safe_Delete_Array(T& ptr)
	{
		if (nullptr != ptr)
		{
			delete [] ptr;
			ptr = nullptr;
		}
	}

	template<typename T>
	unsigned int Safe_AddRef(T& pInstance)
	{
		unsigned int		iRefCnt = 0;

		if (nullptr != pInstance)
		{
			iRefCnt = pInstance->AddRef();
		}
		return iRefCnt;
	}

	template<typename T>
	unsigned long Safe_Release(T& pInstance)
	{
		unsigned long		dwRefCnt = 0;

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}

		return dwRefCnt;
	}
}

#endif // Engine_Function_h__
