

#include "Definition.hlsli"

// r, g, b, a
VertexOutRTT main(VertexIn pin) 
{
    VertexOutRTT vout;
    vout.pos_h = float4(pin.pos_l, 1.0f);  
    vout.uv_l = pin.uv_l;
    return vout;

}