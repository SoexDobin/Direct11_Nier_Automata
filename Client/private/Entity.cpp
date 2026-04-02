#include "pch.h"
#include "Entity.h"

Entity::Entity() : ContainerObject{} {}
Entity::Entity(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: ContainerObject{device, context} {}
Entity::Entity(const Entity& rhs)
	: ContainerObject{rhs} {}
