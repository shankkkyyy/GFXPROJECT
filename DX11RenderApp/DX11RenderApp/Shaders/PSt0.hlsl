

#include "Functions.hlsli"

// r, g, b, a
float4 main(VertexOut pin)  : SV_TARGET
{
    float3 toEye = gEyePosition - pin.pos_w;
    toEye = normalize(toEye);
    return ThreeLightShading(pin, objMaterial, toEye);
}