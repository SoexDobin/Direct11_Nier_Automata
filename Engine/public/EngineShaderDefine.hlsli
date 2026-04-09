
RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockWise = false;
};

RasterizerState RS_Cull_ClockWise
{
    FillMode = Solid;
    CullMode = Front;
    FrontCounterClockWise = false;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = All;
    DepthFunc = less_equal;
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    
    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};

BlendState BS_Screen
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    
    // Final = Src * (1 - Dst) + Dst = Src + Dst - Src*Dst
    SrcBlend = Inv_Dest_Color;
    DestBlend = One;
    BlendOp = Add;
};

BlendState BS_PremultipliedAlpha
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    
    SrcBlend = One;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};

BlendState BS_Additive
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = Src_Alpha;
    DestBlend = One;
    BlendOp = Add;
};