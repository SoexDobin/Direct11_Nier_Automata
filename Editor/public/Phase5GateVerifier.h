#pragma once

NS_BEGIN(Editor)

namespace Phase5GateVerifier
{
	uint32 Get_RequestedStage();
	HRESULT Initialize_Runtime(uint32 stage);
	HRESULT Run(uint32 stage, HRESULT initializationResult);
}

NS_END
