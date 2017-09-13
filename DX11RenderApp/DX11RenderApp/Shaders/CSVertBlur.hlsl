

#include "Resource.hlsli"
#include "CSResource.hlsli"

[numthreads(1, N, 1)]
void main(int3 groupThreadID :SV_GroupThreadID, int3 dispatchThreadID : SV_DispatchThreadID)
{
    uint width, height;
    RTT.GetDimensions(width, height);

    [branch]
    if (groupThreadID.y < gBlurRadius)
    {
        gBlurCache[groupThreadID.y] = RTT[int2(dispatchThreadID.x, max(dispatchThreadID.y - gBlurRadius, 0))];
    }
    else if(groupThreadID.y >= N - gBlurRadius)
    {
        gBlurCache[groupThreadID.y + 2 * gBlurRadius] = RTT[int2(dispatchThreadID.x, min(dispatchThreadID.y + gBlurRadius, height - 1))];

    }
    gBlurCache[groupThreadID.y + gBlurRadius] = RTT[int2(dispatchThreadID.x, min(height - 1, dispatchThreadID.y))];


    // Wait

    GroupMemoryBarrierWithGroupSync();
    // Blur
    float4 blurColor = 0;
    [unroll]
    for (int i = - gBlurRadius; i <= gBlurRadius; i++)
    {
        blurColor += gBlurWeight[i + gBlurRadius] * gBlurCache[i + gBlurRadius + groupThreadID.y];
    }
    PostProcOutput[dispatchThreadID.xy] = blurColor;

}