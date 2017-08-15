

#include "ShaderHeaders.hlsli"





float4 main(VertexOutPos pin) : SV_TARGET
{
    return skyBox.Sample(ss, pin.pos_l);
}