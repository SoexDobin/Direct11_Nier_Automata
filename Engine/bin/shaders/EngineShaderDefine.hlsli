RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState RS_CullClockWise
{
    FillMode = Solid;
    CullMode = Front;
    FrontCounterClockwise = false;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = All;
    DepthFunc = Less_Equal;
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = Zero;
};

BlendState BS_Default
{
    BlendEnable[0] = false;
    BlendEnable[1] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
    SrcBlendAlpha = One;
    DestBlendAlpha = Inv_Src_Alpha;
    BlendOpAlpha = Add;
};

BlendState BS_Additive
{
    BlendEnable[0] = true;
    SrcBlend = One;
    DestBlend = One;
    BlendOp = Add;
    SrcBlendAlpha = One;
    DestBlendAlpha = One;
    BlendOpAlpha = Add;
};

sampler LinearWrapSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

sampler LinearClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};
