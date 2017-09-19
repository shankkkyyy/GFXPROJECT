#include "Functions.hlsli"


float4 main(VertexOut pin) : SV_TARGET
{

    float3 toEye = gEyePosition - pin.pos_w;
    toEye = normalize(toEye);

    float3 incident = -toEye;

    float3 reflectionDir = reflect(incident, pin.nor_w);
    float4 reflectionColor = cubeMap.Sample(ss, reflectionDir);

    float4 lightColor = ThreeLightShading(pin, objMaterial, toEye);
    
    return (lightColor + reflectionColor) * objMaterial.reflection;
}