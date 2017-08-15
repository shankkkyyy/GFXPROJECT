#include "ShaderHeaders.hlsli"

cbuffer perFrame : register(b0)
{
    float3 gEyePosition;
    float pad;

    Light  gLight;
    float4 gAmbientLight;

}

cbuffer perObject : register(b1)
{
    Material objMaterial;
}




// r, g, b, a
float4 main(VertexOut pin) : SV_TARGET
{

    // Get diffuse texture color 
    float4 diffuseAlbedo = diffuseMap.Sample(ss, pin.uv_w);

    // create material based on diffuse texture
    Material mat = { diffuseAlbedo, objMaterial.fresenlR0, objMaterial.shininess };

    // get eye position for lighting
    float3 toEye = gEyePosition - pin.pos_w;
    toEye = normalize(toEye);

    // calculate light
    float3 ambLightColor = (gAmbientLight * objMaterial.diffuseAlbedo).rgb;
    float3 dirLightColor = ComputeDirectionalLight(gLight, mat, pin.nor_w, toEye);
    //float3 pointLightColor = ComputePointLight(gLight, objMaterial, pin.nor_w, pin.pos_w);
    //float3 spotLight = ComputeSpotLight(gLight, objMaterial, pin.nor_w, pin.pos_w);


    float4 colorOut;
    colorOut.a = objMaterial.diffuseAlbedo.a;

    colorOut.rgb = dirLightColor + ambLightColor;
    return colorOut;
}