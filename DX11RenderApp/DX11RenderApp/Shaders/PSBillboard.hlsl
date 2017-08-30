

#include "Functions.hlsli"

// r, g, b, a
float4 main(GeoOutBB pin)  : SV_TARGET
{

    Material mat;
    // no diffuse map, then use material diffuse color
    [flatten]
    if (objRenderSetting.x == 0)
    {
        mat.diffuseAlbedo = objMaterial.diffuseAlbedo;
        mat.shininess = objMaterial.shininess;
        mat.fresenlR0 = objMaterial.fresenlR0;
    }
    else
    {
        // Get diffuse texture color 
        float3 uvw = { pin.uv, pin.primID % 4 };
        float4 diffuseAlbedo = diffuseMapArray.Sample(ssClamp, uvw) * objMaterial.diffuseAlbedo;

        clip(diffuseAlbedo.a - 0.1f);

        mat.diffuseAlbedo.rgb = diffuseAlbedo.rgb;
        mat.shininess = objMaterial.shininess;
        mat.fresenlR0 = objMaterial.fresenlR0;
    }

    // get eye position for lighting
    float3 toEye = gEyePosition - pin.pos_w;
    float toEyeDistance = length(toEye);
    toEye /= toEyeDistance;

    // calculate light
    float3 ambLightColor = (gAmbientLight * mat.diffuseAlbedo).rgb;
    float3 dirLightColor = ComputeDirectionalLight(gLight[0], mat, pin.nor_w, toEye);
    float3 pointLightColor = ComputePointLight(gLight[1], objMaterial, pin.nor_w, pin.pos_w);
    float3 spotLight = ComputeSpotLight(gLight[2], objMaterial, pin.nor_w, pin.pos_w);



    float4 litColor;
    litColor.rgb = dirLightColor + ambLightColor + pointLightColor + spotLight;
    //litColor.rgb = FogEffect(litColor.rgb, toEyeDistance);
    litColor.a = mat.diffuseAlbedo.a;
    return litColor;
}