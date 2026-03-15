#include "Bone.h"

#include "String_Helper.h"
#include "SpdLogger.h"

Bone::Bone() : Component{}
{
}

Bone::Bone(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Component{device, context}
{
}

HRESULT Bone::Initialize_Prototype(const MODEL_BONE& modelBone)
{
	strcpy_s(m_BoneName, modelBone.name.data());
	m_ParentBoneIndex = modelBone.parentIndex;

	m_TransformationMatrix = modelBone.transform;
	m_CombinedTransformationMatrix = Matrix::Identity;

	return Component::Initialize_Prototype();
}

HRESULT Bone::Initialize_Prototype()
{
	return Component::Initialize_Prototype();
}

HRESULT Bone::Initialize(void* arg)
{
	return Component::Initialize(arg);
}

void Bone::Update_CombinedTransformationMatrix(const vector<Shared<Bone>>& modelBones, const Matrix& preTransformMatrix)
{
	if (-1 == m_ParentBoneIndex)
		m_CombinedTransformationMatrix = m_TransformationMatrix * preTransformMatrix;
	else
		m_CombinedTransformationMatrix = m_TransformationMatrix * modelBones[m_ParentBoneIndex]->m_CombinedTransformationMatrix;
}

Shared<Bone> Bone::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const MODEL_BONE& boneData)
{
	auto bone = make_shared<Bone>(device, context);

	if (FAILED(bone->Initialize_Prototype(boneData)))
	{
		LOG_ERROR(L"Failed to Created : Bone {}", Helper::To_wString(boneData.name));
		MSG_BOX("Failed to Created : Bone");
	}
	return bone;
}