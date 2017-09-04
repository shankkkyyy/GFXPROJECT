
#include "Resource.hlsli"





float3 FogEffect(float3 _litColor, float _toEyeDistance)
{
    float ratio = saturate((_toEyeDistance - gFogStart) / (gFogRange));
    // Add Fog
    _litColor = lerp(_litColor, gFogColor, ratio);
    return _litColor;
}

/* we are only considering reflection for now*/
float3 ComputeSpecularLightFactor(Material mat, float3 incomingLightDir, float3 toEye, float3 nor)
{

    float3 specularFactor = 0.0f;


    float3 reflection = normalize(reflect(incomingLightDir, nor));

    float ks = max(dot(reflection, toEye) , 0);

    [flatten]
    if  (ks != 0)
    {
        ks = pow(ks, mat.shininess);

        specularFactor = ks * mat.fresenlR0;
    }

    return specularFactor;
}

float3 ComputeDirectionalLight(Light Ld, Material mat, float3 nor, float3 toEye)
{
    float3 lightStrength = 0.0f;
    float kd = max(dot(-Ld.direction, nor), 0);

	[flatten]
    if (kd != 0)
    {
        float3 ks = ComputeSpecularLightFactor(mat, Ld.direction, toEye, nor) * Ld.lightColor;
        float3 lightFactor = kd + ks;
        lightStrength = lightFactor * Ld.lightColor;
    }

    return lightStrength;
}

float3 ComputePointLight(Light Lp, Material mat, float3 nor, float3 pos, float3 toEye)
{
    float3 lightStrength = 0.0f;

    // make a range check
    float3 lightSourceDir = Lp.position - pos;
    float distance = length(lightSourceDir);

    [flatten]
    if (distance > Lp.fallOffEnd)
        return lightStrength;

    lightSourceDir = normalize(lightSourceDir);


    float kd = max(dot(lightSourceDir, nor), 0);

    [flatten]
    if (kd == 0)
    {
        return lightStrength;
    }

    float3 ks = ComputeSpecularLightFactor(mat, -lightSourceDir, toEye, nor);

    float3 lightFactor = kd + ks;

    float att = min((Lp.fallOffEnd - distance) / (Lp.fallOffEnd - Lp.fallOffStart), 1.0f);

    lightStrength = lightFactor * att * Lp.lightColor;


    return lightStrength;
}

float3 ComputeSpotLight(Light Ls, Material mat, float3 nor, float3 pos, float3 toEye)
{
    float3 lightStrength = 0.0f;

    // make a range check
    float3 lightSourceDir = Ls.position - pos;
    float distance = length(lightSourceDir);

    [flatten]
    if (distance > Ls.fallOffEnd)
        return lightStrength;

    lightSourceDir = normalize(lightSourceDir);

    float kd = max(dot(lightSourceDir, nor), 0);
    [flatten]
    if (kd == 0)
        return lightStrength;

    float kspot = max(dot(Ls.direction, -lightSourceDir), 0);

    [flatten]
    if (kspot < Ls.spotAngle)
        return lightStrength;

    kspot = 1 - (1 - kspot) / (1 - Ls.spotAngle);
    //float att = 1;
    float att = (Ls.fallOffEnd - distance) / (Ls.fallOffEnd - Ls.fallOffStart);

    float3 ks = ComputeSpecularLightFactor(mat, -lightSourceDir, toEye, nor);

    float3 lightFactor = (kspot + ks)*att;

    lightStrength = lightFactor * Ls.lightColor;

    return lightStrength;

}

