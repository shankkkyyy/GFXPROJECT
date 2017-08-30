
#include "Resource.hlsli"

[MaxVertexCount(4)]
void main(point VertexIOBB gin[1], uint primID : SV_PrimitiveID, inout TriangleStream<GeoOutBB> triStream)
{
    // get quad vertices in local space from a point
    
    const float3 up = { 0, 1, 0 };
    float3 forward = gEyePos.xyz - gin[0].root_w;
    forward = normalize(forward);


    // x z y x z
    float3 right = cross(forward, up);

    float halfWidth  = 0.5f * gin[0].size.x;
    float height = gin[0].size.y;
    
    // 0 1
    // 3 2

    float2 uvs[4] =
    {
        float2(0, 1),
        float2(0, 0),
        float2(1, 1),
        float2(1, 0)
    };


    float4 v[4] =
    {
        { gin[0].root_w - right * halfWidth, 1.0f },   //3
        { gin[0].root_w - right * halfWidth + up * height, 1.0f },
        { gin[0].root_w + right * halfWidth, 1.0f },
        { gin[0].root_w + right * halfWidth + up * height, 1.0f }
    };

    GeoOutBB gOut;

    [unroll]
    for (uint i = 0; i < 4; ++i)
    {
        gOut.pos_w = v[i].xyz;
        gOut.pos_h = mul(v[i], gViewProjGS);
        gOut.nor_w = forward;
        gOut.primID = primID;
        gOut.uv = uvs[i];
        triStream.Append(gOut);
    }


}