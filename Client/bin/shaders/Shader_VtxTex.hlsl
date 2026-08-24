#include "EngineShaderDefine.hlsli"

float4x4 g_WorldMatrix;
float4x4 g_ViewMatrix;
float4x4 g_ProjMatrix;
texture2D g_Texture;
float4 g_RGBA = float4(1.f, 1.f, 1.f, 1.f);
float g_HpRatio = 1.f;

struct VS_IN { float3 position : POSITION; float2 texcoord : TEXCOORD0; };
struct VS_OUT { float4 position : SV_POSITION; float2 texcoord : TEXCOORD0; };

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;
    output.position = mul(float4(input.position, 1.f), mul(mul(g_WorldMatrix, g_ViewMatrix), g_ProjMatrix));
    output.texcoord = input.texcoord;
    return output;
}

float4 PS_Default(VS_OUT input) : SV_TARGET0
{
    return g_Texture.Sample(LinearClampSampler, input.texcoord) * g_RGBA;
}

float4 PS_HpBar(VS_OUT input) : SV_TARGET0
{
    if (input.texcoord.x > saturate(g_HpRatio)) discard;
    return g_Texture.Sample(LinearClampSampler, input.texcoord) * g_RGBA;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_Main();
        PixelShader = compile ps_5_0 PS_Default();
    }
    pass HpBar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_Main();
        PixelShader = compile ps_5_0 PS_HpBar();
    }
}
