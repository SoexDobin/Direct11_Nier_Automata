#include "EngineShaderDefine.hlsli"

float4x4 g_WorldMatrix;
float4x4 g_ViewMatrix;
float4x4 g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;

struct VS_IN { float3 position : POSITION; float3 normal : NORMAL; float3 tangent : TANGENT; float2 texcoord : TEXCOORD0; };
struct VS_OUT { float4 position : SV_POSITION; float3 normal : NORMAL; float2 texcoord : TEXCOORD0; };
struct PS_OUT { float4 diffuse : SV_TARGET0; float4 normal : SV_TARGET1; };

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;
    output.position = mul(float4(input.position, 1.f), mul(mul(g_WorldMatrix, g_ViewMatrix), g_ProjMatrix));
    output.normal = normalize(mul(float4(input.normal, 0.f), g_WorldMatrix).xyz);
    output.texcoord = input.texcoord;
    return output;
}

PS_OUT PS_Main(VS_OUT input)
{
    PS_OUT output;
    output.diffuse = g_DiffuseTexture.Sample(LinearWrapSampler, input.texcoord);
    if (output.diffuse.a < 0.3f) discard;
    output.normal = float4(normalize(input.normal) * 0.5f + 0.5f, 1.f);
    return output;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_Main(); PixelShader = compile ps_5_0 PS_Main();
    }
    pass AlphaBlend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_Main(); PixelShader = compile ps_5_0 PS_Main();
    }
    pass ShadowCompatible
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_Main(); PixelShader = compile ps_5_0 PS_Main();
    }
}
