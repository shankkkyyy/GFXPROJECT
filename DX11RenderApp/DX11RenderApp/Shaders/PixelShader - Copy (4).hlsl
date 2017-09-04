

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

    return ThreeLightShading(pin, mat);
}