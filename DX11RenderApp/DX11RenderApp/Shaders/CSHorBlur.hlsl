

#include "Resource.hlsli"
#include "CSResource.hlsli"

[numthreads(N, 1, 1)]
void main(int3 groupThreadID :SV_GroupThreadID, int3 dispatchThreadID :SV_DispatchThreadID)
{

    // clmap 
    int width, height;
    RTT.GetDimensions(width, height);

    [branch]
    if (groupThreadID.x < gBlurRadius)
    {
        int x = max(dispatchThreadID.x - gBlurRadius, 0);
        gBlurCache[groupThreadID.x] = RTT[int2(x, dispatchThreadID.y)];
    }
    else if (groupThreadID.x >= N - gBlurRadius)
    {
        int x = min(dispatchThreadID.x + gBlurRadius, width - 1);
        gBlurCache[groupThreadID.x + 2 * gBlurRadius] = RTT[int2(x, dispatchThreadID.y)];
    }

    gBlurCache[groupThreadID.x + gBlurRadius] = RTT[int2(min(dispatchThreadID.x, width - 1), dispatchThreadID.y)];

    // wait 
    GroupMemoryBarrierWithGroupSync();

    // blur
    float4 blurColor = 0.0f;
    [unroll]
    for (int i = -gBlurRadius; i <= gBlurRadius; i ++)
    {
        blurColor += gBlurCache[groupThreadID.x + gBlurRadius + i] * gBlurWeight[i + gBlurRadius];
    }

    PostProcOutput[dispatchThreadID.xy] = blurColor;
}