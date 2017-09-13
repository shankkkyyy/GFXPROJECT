

#include "Functions.hlsli"

// r, g, b, a
float4 main(VertexOutRTT pin)  : SV_TARGET
{

    float4 RTTColor = RTT.Sample(ssClamp, pin.uv_l);

    if (gPSSettings.x == RED)
        return float4(RTTColor.r, 0, 0, 1.0f);
    else if (gPSSettings.x == GREEN)
        return float4(0, RTTColor.g, 0, 1.0f);
    else if (gPSSettings.x == BLUE)
        return float4(0, 0, RTTColor.b, 1.0f);
    else
        return RTTColor;

}