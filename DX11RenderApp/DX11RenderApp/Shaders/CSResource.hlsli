// For Blur

cbuffer CSBlurFixed
{
    static const float gBlurWeight[9] =
    { 0.05f, 0.05f, 0.1f, 0.15f, 0.3f, 0.15f, 0.1f, 0.05f, 0.05f };
    static const int gBlurRadius = 4;
};


#define N 256
#define CacheSize  (N + 2 * gBlurRadius) 

groupshared float4 gBlurCache[CacheSize];