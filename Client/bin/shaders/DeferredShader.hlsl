#include "EngineShaderDefine.hlsli"

float4x4 g_WorldMatrix;
float4x4 g_ViewMatrix;
float4x4 g_ProjMatrix;

texture2D g_Texture;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_ShadeTexture;

float4 g_LightDirection = float4(0.f, -1.f, 0.f, 0.f);

struct VS_IN
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;
    float4x4 worldViewProjection = mul(mul(g_WorldMatrix, g_ViewMatrix), g_ProjMatrix);
    output.position = mul(float4(input.position, 1.f), worldViewProjection);
    output.texcoord = input.texcoord;
    return output;
}

float4 PS_Debug(VS_OUT input) : SV_TARGET0
{
    return g_Texture.Sample(LinearClampSampler, input.texcoord);
}

float4 PS_Directional(VS_OUT input) : SV_TARGET0
{
    float3 encodedNormal = g_NormalTexture.Sample(LinearClampSampler, input.texcoord).xyz;
    float3 normal = normalize(encodedNormal * 2.f - 1.f);
    float diffuse = saturate(dot(normal, -normalize(g_LightDirection.xyz)));
    return float4(0.2f + diffuse, 0.2f + diffuse, 0.2f + diffuse, 1.f);
}

float4 PS_Point(VS_OUT input) : SV_TARGET0
{
    // Point-light position and range are not bound by Light::Render yet.
    return float4(0.f, 0.f, 0.f, 0.f);
}

float4 PS_Combined(VS_OUT input) : SV_TARGET0
{
    float4 diffuse = g_DiffuseTexture.Sample(LinearClampSampler, input.texcoord);
    float4 shade = g_ShadeTexture.Sample(LinearClampSampler, input.texcoord);
    return float4(diffuse.rgb * shade.rgb, diffuse.a);
}

technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Debug();
    }

    pass Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Directional();
    }

    pass Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Point();
    }

    pass Combined
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Combined();
    }
}
