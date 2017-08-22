
#include "Resource.hlsli"





float3 FogEffect(float3 _litColor, float _toEyeDistance)
{
    float ratio = saturate((_toEyeDistance - gFogStart) / (gFogRange));
    // Add Fog
    _litColor = lerp(_litColor, gFogColor, ratio);
    return _litColor;
}

/* we are only considering reflection for now*/
float3 ComputeSpecularLighting(Material mat, float3 lightSourceDir, float3 toEye, float3 nor)
{
    float3 specularAlbedo = 0.0f;

    float3 halfVector = normalize(lightSourceDir + toEye);

    float cosTheta = saturate(dot(halfVector, lightSourceDir));

    float3 fresnelFactor = mat.fresenlR0 + (1 - mat.fresenlR0) * pow(1 - cosTheta, 5);


    float shininessFactor = max(dot(halfVector, nor), 0);
    [fallten]
    if (shininessFactor == 0)
        return specularAlbedo;

    shininessFactor = pow(shininessFactor, mat.shininess);

    specularAlbedo = fresnelFactor * shininessFactor;

    return specularAlbedo;
}

float3 ComputeDirectionalLight(Light Ld, Material mat, float3 nor, float3 toEye)
{
    float3 lightStrength = 0.0f;
    float kd = max(dot(-Ld.direction, nor), 0);

	[flatten]
    if (kd != 0)
    {
        lightStrength = kd * Ld.lightColor;
    }

    float3 colorOut = lightStrength * (mat.diffuseAlbedo.rgb); // B * (d + s);
    return colorOut;
}

float3 ComputePointLight(Light Lp, Material mat, float3 nor, float3 pos)
{
    float3 lightStrengh = 0.0f;

    // make a range check
    float3 lightSourceDir = Lp.position - pos;
    float distance = length(lightSourceDir);

    [flatten]
    if (distance > Lp.fallOffEnd)
        return lightStrengh;

    lightSourceDir = normalize(lightSourceDir);


    float kd = max(dot(lightSourceDir, nor), 0);

    [flatten]
    if (kd == 0)
        return lightStrengh;


    float att = (Lp.fallOffEnd - distance) / (Lp.fallOffEnd - Lp.fallOffStart);

    lightStrengh = att * kd * Lp.lightColor;
    
    return lightStrengh;
}

float3 ComputeSpotLight(Light Ls, Material mat, float3 nor, float3 pos)
{
    float3 colorOut = 0.0f;

    // make a range check
    float3 lightSourceDir = Ls.position - pos;
    float distance = length(lightSourceDir);

    [flatten]
    if (distance > Ls.fallOffEnd)
        return colorOut;

    lightSourceDir = normalize(lightSourceDir);

    float kd = max(dot(lightSourceDir, nor), 0);
    [flatten]
    if (kd == 0)
        return colorOut;


    float kspot = max(dot(Ls.direction, -lightSourceDir), 0);

    [flatten]
    if (kspot < Ls.spotAngle)
        return colorOut;

    kspot = 1 - (1 - kspot) / (1 - Ls.spotAngle);
    //float att = 1;
    float att = (Ls.fallOffEnd - distance) / (Ls.fallOffEnd - Ls.fallOffStart);

    colorOut = kspot * att * Ls.lightColor * mat.diffuseAlbedo.rgb; //
    return colorOut;


}

