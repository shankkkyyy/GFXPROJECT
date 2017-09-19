

#include "TessResource.hlsli"




[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("UniformQuad")]
[maxtessfactor(64.0f)]

VertexTerrian main(InputPatch<VertexTerrian, 4> p,
uint i : SV_OutputControlPointID, 
uint patchID : SV_PrimitiveID)
{
    VertexTerrian hsOut;
    hsOut = p[i];
    return hsOut;
}