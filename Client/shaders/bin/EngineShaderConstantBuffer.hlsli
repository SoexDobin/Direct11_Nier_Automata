#ifndef ENGINE_SHADER_CONSTANT_BUFFER_HLSLI
#define ENGINE_SHADER_CONSTANT_BUFFER_HLSLI

cbuffer MaterialBuffer
{
    float4 g_BaseColorTint;
    float4 g_EmissiveColor;
    float g_EmissiveIntensity;
    float g_NormalStrength;
    float g_OpacityMaskClipValue;
    float g_AoScale;
    float g_RoughnessScale;
    float g_MetalnessScale;
    uint g_TextureMask;
    float g_MaterialPadding;
};

cbuffer CameraBuffer
{
    row_major float4x4 g_ViewMatrix;
    row_major float4x4 g_ProjMatrix;
    row_major float4x4 g_ViewInverseMatrix;
    row_major float4x4 g_ProjInverseMatrix;
    float4 g_CameraPosition;
};

cbuffer ObjectBuffer
{
    row_major float4x4 g_WorldMatrix;
};

cbuffer LightBuffer
{
    float4 g_LightDir;
    float4 g_LightPosition;
    float4 g_LightDiffuse;
    float4 g_LightAmbient;
    float4 g_LightSpecular;
    float g_LightRange;
    float g_LightAttenuationPower;
    float2 g_LightPadding;
};

#endif
