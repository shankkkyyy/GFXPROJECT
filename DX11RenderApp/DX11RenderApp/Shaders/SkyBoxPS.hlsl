

#include "Functions.hlsli"





float4 main(VertexOutPos pin) : SV_TARGET
{
    //float3 toEye = gEyePosition - pin.pos_w;
    //float toEyeDistance = length(toEye);

    //float4 litColor = skyBox.Sample(ss, pin.pos_l);



    return skyBox.Sample(ss, pin.pos_l);
}