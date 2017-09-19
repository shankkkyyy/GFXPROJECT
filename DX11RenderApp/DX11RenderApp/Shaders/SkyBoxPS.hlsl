

#include "Functions.hlsli"





float4 main(VertexOutPos pin) : SV_TARGET
{


    return cubeMap.Sample(ss, pin.pos_l);
}