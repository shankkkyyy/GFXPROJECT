

#include "Functions.hlsli"

// r, g, b, a
float4 main(VertexOut pin)  : SV_TARGET
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
        float4 diffuseAlbedo = diffuseMap.Sample(ss, pin.uv_w);
        mat.diffuseAlbedo.rgb = diffuseAlbedo.rgb;
        mat.shininess = objMaterial.shininess;
        mat.fresenlR0 = objMaterial.fresenlR0;
    }

    // get eye position for lighting
    float3 toEye = gEyePosition - pin.pos_w;
    float toEyeDistance = length(toEye);
    toEye /= toEyeDistance;

    // calculate light
    float3 ambLight   = (gAmbientLight * mat.diffuseAlbedo).rgb;
    float3 dirLight   = ComputeDirectionalLight(gLight[0], mat, pin.nor_w, toEye);
    float3 pointLight = ComputePointLight(gLight[1], objMaterial, pin.nor_w, pin.pos_w, toEye);
    float3 spotLight  = ComputeSpotLight(gLight[2], objMaterial, pin.nor_w, pin.pos_w, toEye);

    float4 litColor;
    litColor.rgb = (ambLight + +dirLight + pointLight + spotLight) * mat.diffuseAlbedo.rgb;
    //litColor.rgb = FogEffect(litColor.rgb, toEyeDistance);
    litColor.a = mat.diffuseAlbedo.a;
    return litColor;
}