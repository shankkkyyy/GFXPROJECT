
#include "Resource.hlsli"

#define divBy3 1.0f/3.0f

struct PatchTess
{
    float EdgeTess[3]    : SV_TessFactor;
    float InnerTess[1]   : SV_InsideTessFactor;
};

PatchTess UniformQuad(InputPatch<VertexInPos, 3> patch, uint patchID:SV_PrimitiveID)
{

    float3 pos_l = divBy3 * (patch[0].pos_l + patch[1].pos_l + patch[2].pos_l); // + patch[3].pos_l);

    // assuming its at original 
    float3 pos_w = pos_l;

    float distance = length(gEyePositionHS - pos_w);

    const float dMin = 10.0f, dMax = 100.0f;

    float tess = 64.0f * saturate((dMax - distance) / (dMax - dMin));

    PatchTess pOut;

    pOut.EdgeTess[0] = tess;
    pOut.EdgeTess[1] = tess;
    pOut.EdgeTess[2] = tess;
    //pOut.EdgeTess[3] = tess;
    
    pOut.InnerTess[0] = tess;
    //pOut.InnerTess[1] = tess;

    return pOut;

}