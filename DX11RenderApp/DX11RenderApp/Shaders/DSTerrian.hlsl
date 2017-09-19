

#include "TessResource.hlsli"

[domain("quad")]
DomainOut main(PatchTess patchTess, float2 uv:SV_DomainLocation, 
const OutputPatch<VertexTerrian, 4> quad)
{
    DomainOut dOut;

    float3 u0 = lerp(quad[0].pos_l, quad[1].pos_l, uv.x);
    float3 u1 = lerp(quad[2].pos_l, quad[3].pos_l, uv.x);
    float3 v0 = lerp(u0, u1, uv.y);

    dOut.pos_h = mul(float4(v0, 1.0f), gDSwvp);

    return dOut;
}