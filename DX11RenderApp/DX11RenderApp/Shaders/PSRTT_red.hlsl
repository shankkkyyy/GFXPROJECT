

#include "Functions.hlsli"

// r, g, b, a
float4 main(VertexOutRTT pin)  : SV_TARGET
{

    float4 RTTColor = RTT.Sample(ssClamp, pin.uv_l);

    return float4(RTTColor.r, 0, 0, 1.0f);

}