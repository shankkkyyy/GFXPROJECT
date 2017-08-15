


#include "ShaderHeaders.hlsli"

cbuffer cbPerObject : register(b1)
{
    float4x4 objWorldViewProject;
    float4x4 objWorld;
    float4x4 objInvTranspose;
    float4x4 objTexTransform;
}


VertexOut main( VertexIn vin) 
{
    VertexOut vout;

    vout.pos_h = mul(float4(vin.pos_l, 1.0f), objWorldViewProject);
    vout.pos_w = mul(float4(vin.pos_l, 1.0f), objWorld).xyz;
    vout.nor_w = normalize( mul(float4(vin.nor_l, 1.0f), objInvTranspose).xyz);
    vout.uv_w = mul(float4(vin.uv_l, 0.0f, 1.0f), objTexTransform).xy;

	return vout;
}