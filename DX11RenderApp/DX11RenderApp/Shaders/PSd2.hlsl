

#include "Functions.hlsli"

// r, g, b, a
float4 main(VertexOut pin)  : SV_TARGET
{

    Material mat;


    float4 diffuseAlbedo = diffuseMap.Sample(ss, pin.uv_w);
    float4 diffuseAlbedo1 = diffuseMap1.Sample(ss, pin.uv_w);


    mat.diffuseAlbedo.rgb = saturate(diffuseAlbedo.rgb * diffuseAlbedo1.rgb * 2);
    mat.diffuseAlbedo.a = objMaterial.diffuseAlbedo.a;
    mat.shininess = objMaterial.shininess;
    mat.fresenlR0 = objMaterial.fresenlR0;

    return ThreeLightShading(pin, mat);
}