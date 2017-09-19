

#include "Definition.hlsli"

cbuffer VSPerFrame : register(b0)
{
    float4x4 gCamPosTransform;  // for skybox
    float4x4 gViewProj;
}

cbuffer VSPerObject : register(b1)
{
    float4x4 objWorldViewProject;
    float4x4 objWorld;
    float4x4 objTexTransform;
}

cbuffer GSPerFrame : register(b0)
{
    float4   gEyePos;
    float4x4 gViewProjGS;
}

cbuffer PSPerFrame : register(b0)
{
    float3 gEyePosition;
    float gFogRange;
    float3 gFogColor;
    float gFogStart;
    Light gLight[3];
    float4 gAmbientLight;
    uint4 gPSSettings;

}

cbuffer PSPerObject : register(b1)
{
    Material  objMaterial;
    int4 objRenderSetting;
}

cbuffer HSPerFrame : register(b0)
{
    float3 gEyePositionHS;
    float pad_0;
}

cbuffer DSPerFrame : register(b0)
{
    float4x4 gDSwvp;
}



Texture2D diffuseMap : register(t0);
Texture2D diffuseMap1 : register(t1);


Texture2D RTT : register(t9);                     // For Blur
TextureCube cubeMap : register(t10);
Texture2DArray diffuseMapArray : register(t11);
Texture2D heightMap : register(t12);


RWTexture2D<unorm float4> PostProcOutput : register(u0); // For BLur

SamplerState ss : register(s0);
SamplerState ssClamp : register(s1);


