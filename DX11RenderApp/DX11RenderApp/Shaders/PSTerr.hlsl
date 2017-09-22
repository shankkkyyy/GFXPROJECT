

#include "Functions.hlsli"

// r, g, b, a
float4 main(DomainOut pin)  : SV_TARGET
{
    // sampling
    float4 color0 = diffuseMapArray.Sample(ss, float3(pin.uvLayer, 0));
    float4 color1 = diffuseMapArray.Sample(ss, float3(pin.uvLayer, 1));
    float4 color2 = diffuseMapArray.Sample(ss, float3(pin.uvLayer, 2));
    float4 color3 = diffuseMapArray.Sample(ss, float3(pin.uvLayer, 3));
    float4 color4 = diffuseMapArray.Sample(ss, float3(pin.uvLayer, 4));

    float4 blendFactor = blendMap.Sample(ss, pin.uvHeight);
    float4 texColor = color0;
    texColor = lerp(texColor, color1, blendFactor.r);
    texColor = lerp(texColor, color2, blendFactor.g);
    texColor = lerp(texColor, color3, blendFactor.b);
    texColor = lerp(texColor, color4, blendFactor.a);

    return texColor;
}

