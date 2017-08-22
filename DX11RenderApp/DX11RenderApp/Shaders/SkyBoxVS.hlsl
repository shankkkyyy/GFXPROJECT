

#include "Resource.hlsli"

VertexOutPos main(VertexInPos vin) 
{
    VertexOutPos vout;
    const float4x4 I
    = { 
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
    vout.pos_h = mul(float4(vin.pos_l, 1.0f), gCamPosTransform).xyww;
    vout.pos_l = vin.pos_l;

    return vout;
}