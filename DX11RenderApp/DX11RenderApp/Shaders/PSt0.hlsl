

#include "Functions.hlsli"

// r, g, b, a
float4 main(VertexOut pin)  : SV_TARGET
{
    return ThreeLightShading(pin, objMaterial);
}