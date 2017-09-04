

#include "Resource.hlsli"



VertexOut main(VertexInInst vin)
{
    VertexOut vout;

    vout.pos_w = mul(float4(vin.pos_l, 1.0f), vin.world).xyz;
    vout.pos_h = mul(float4(vout.pos_w, 1.0f), gViewProj);
    vout.nor_w = normalize(mul(vin.nor_l, (float3x3)vin.world));
    vout.uv_w = mul(float4(vin.uv_l, 0.0f, 1.0f), objTexTransform).xy;

    return vout;
}