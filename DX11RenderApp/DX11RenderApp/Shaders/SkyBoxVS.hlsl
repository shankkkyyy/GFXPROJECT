

#include "ShaderHeaders.hlsli"




VertexOutPos main(VertexInPos vin) 
{
    VertexOutPos vout;
    vout.pos_h = mul(float4(vin.pos_l, 1.0f), gCameraWVP);
    vout.pos_l = vin.pos_l;
    return vout;
}