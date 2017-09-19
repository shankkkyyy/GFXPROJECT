
#include "Resource.hlsli"

#define divBy3 1.0f/3.0f

struct PatchTess
{
    float EdgeTess[4]    : SV_TessFactor;
    float InnerTess[2]   : SV_InsideTessFactor;
};


float CalculateTessFactor(float distance)
{
    const float dMin = 25.0f, dMax = 250.0f;

    float ratio = saturate((distance - dMin) / (dMax - dMin));

    float tess = pow(2, lerp(6, 0, ratio));

    return tess;
};

PatchTess UniformQuad(InputPatch<VertexTerrian, 4> patch, uint patchID:SV_PrimitiveID)
{

    // assuming its at original 

    // 0 1 1
    // 0   2
    // 2 3 3



    float3 pos_w[4];

    pos_w[0] = 0.5f * (patch[0].pos_l + patch[2].pos_l);
    pos_w[1] = 0.5f * (patch[0].pos_l + patch[1].pos_l);
    pos_w[2] = 0.5f * (patch[1].pos_l + patch[3].pos_l);
    pos_w[3] = 0.5f * (patch[2].pos_l + patch[3].pos_l);

    float distance[4];

    distance[0] = length(pos_w[0] - gEyePositionHS);
    distance[1] = length(pos_w[1] - gEyePositionHS);
    distance[2] = length(pos_w[2] - gEyePositionHS);
    distance[3] = length(pos_w[3] - gEyePositionHS);


    float3 center = (patch[0].pos_l + patch[2].pos_l + patch[1].pos_l + patch[3].pos_l) * 0.25f;
    float centerTess = CalculateTessFactor(length(center - gEyePositionHS));

    PatchTess pOut;

    pOut.EdgeTess[0] = CalculateTessFactor(distance[0]);
    pOut.EdgeTess[1] = CalculateTessFactor(distance[1]);
    pOut.EdgeTess[2] = CalculateTessFactor(distance[2]);
    pOut.EdgeTess[3] = CalculateTessFactor(distance[3]);
    

    pOut.InnerTess[0] = centerTess;
    pOut.InnerTess[1] = centerTess;

    return pOut;
}