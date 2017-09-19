

#include "Functions.hlsli"

// r, g, b, a
float4 main(VertexOut pin)  : SV_TARGET
{

    Material mat;
    // no diffuse map, then use material diffuse color
    float4 diffuseAlbedo = diffuseMap.Sample(ss, pin.uv_w);

    mat.diffuseAlbedo.a = objMaterial.diffuseAlbedo.a;
    mat.diffuseAlbedo.rgb = diffuseAlbedo.rgb;
    mat.shininess = objMaterial.shininess;
    mat.fresenlR0 = objMaterial.fresenlR0;

    float3 toEye = gEyePosition - pin.pos_w;
    toEye = normalize(toEye);

    return ThreeLightShading(pin, mat, toEye);
}