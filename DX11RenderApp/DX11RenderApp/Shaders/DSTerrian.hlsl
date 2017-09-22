

#include "TessResource.hlsli"

[domain("quad")]
DomainOut main(PatchTess patchTess, float2 domainLocation:SV_DomainLocation, 
const OutputPatch<VertexTerrian, 4> quad)
{
    DomainOut dOut;

 
    dOut.pos_w = lerp(
                 lerp(quad[0].pos_l, quad[1].pos_l, domainLocation.x),
                 lerp(quad[2].pos_l, quad[3].pos_l, domainLocation.x),
                 domainLocation.y);

    dOut.uvHeight = lerp(
                 lerp(quad[0].uv, quad[1].uv, domainLocation.x),
                 lerp(quad[2].uv, quad[3].uv, domainLocation.x),
                 domainLocation.y);

    const float2 texScale = float2(300, 300);

    dOut.uvLayer = dOut.uvHeight * texScale;


    dOut.pos_w.y = heightMap.SampleLevel(ss, dOut.uvHeight, 0).r;

    dOut.pos_h = mul(float4(dOut.pos_w, 1.0f), gDSwvp);
    



    return dOut;
}