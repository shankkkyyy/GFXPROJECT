

#include "TessResource.hlsli"




[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("UniformQuad")]
[maxtessfactor(64.0f)]

VertexInPos main(InputPatch<VertexInPos, 3> p, 
uint i : SV_OutputControlPointID, 
uint patchID : SV_PrimitiveID)
{
    VertexInPos hsOut;
    hsOut.pos_l = p[i].pos_l;
    return hsOut;
}