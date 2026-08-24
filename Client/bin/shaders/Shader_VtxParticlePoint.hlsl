#include "EngineShaderDefine.hlsli"

float4x4 g_WorldMatrix;
float4x4 g_ViewMatrix;
float4x4 g_ProjMatrix;
float4 g_CameraPosition;
uint g_LockUpRight;
texture2D g_DiffuseTexture;

struct VS_IN
{
    float3 position : POSITION;
    float4 world0 : WORLD0;
    float4 world1 : WORLD1;
    float4 world2 : WORLD2;
    float4 world3 : WORLD3;
    float2 lifetime : TEXCOORD0;
};
struct VS_OUT { float4 position : POSITION; float2 lifetime : TEXCOORD0; float2 size : TEXCOORD1; };
struct GS_OUT { float4 position : SV_POSITION; float2 texcoord : TEXCOORD0; float alpha : TEXCOORD1; };

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;
    float4x4 instanceWorld = float4x4(input.world0, input.world1, input.world2, input.world3);
    output.position = mul(mul(float4(input.position, 1.f), instanceWorld), g_WorldMatrix);
    output.lifetime = input.lifetime;
    output.size = float2(length(input.world0.xyz), length(input.world1.xyz));
    return output;
}

[maxvertexcount(4)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> stream)
{
    float3 center = input[0].position.xyz;
    float3 look = normalize(g_CameraPosition.xyz - center);
    float3 up = (g_LockUpRight != 0) ? float3(0.f, 1.f, 0.f) : normalize(float3(g_ViewMatrix._12, g_ViewMatrix._22, g_ViewMatrix._32));
    float3 right = normalize(cross(up, look));
    up = normalize(cross(look, right));
    float2 corners[4] = { float2(-0.5f, 0.5f), float2(0.5f, 0.5f), float2(-0.5f, -0.5f), float2(0.5f, -0.5f) };
    float2 uvs[4] = { float2(0.f, 0.f), float2(1.f, 0.f), float2(0.f, 1.f), float2(1.f, 1.f) };
    float alpha = 1.f - saturate(input[0].lifetime.y / max(input[0].lifetime.x, 0.0001f));
    [unroll] for (uint i = 0; i < 4; ++i)
    {
        GS_OUT output;
        float3 worldPosition = center
                             + right * corners[i].x * input[0].size.x
                             + up * corners[i].y * input[0].size.y;
        output.position = mul(float4(worldPosition, 1.f), mul(g_ViewMatrix, g_ProjMatrix));
        output.texcoord = uvs[i];
        output.alpha = alpha;
        stream.Append(output);
    }
}

float4 PS_Main(GS_OUT input) : SV_TARGET0
{
    float4 color = g_DiffuseTexture.Sample(LinearClampSampler, input.texcoord);
    color.a *= input.alpha;
    if (color.a < 0.01f) discard;
    return color;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default); SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_Main();
        PixelShader = compile ps_5_0 PS_Main();
    }
}
