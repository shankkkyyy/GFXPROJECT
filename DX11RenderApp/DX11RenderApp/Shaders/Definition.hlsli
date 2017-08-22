

struct VertexInPos
{
    float3 pos_l : POSITION;
};

struct VertexOutPos
{
    float4 pos_h : SV_Position;
    float3 pos_l : POSITIONL;
    //float3 pos_w : POSITIONW;
};

struct VertexIn
{
    float3 pos_l : POSITION;
    float3 nor_l : NORMAL;
    float2 uv_l : UV;
};
				   	   
struct VertexOut
{
    float4 pos_h : SV_POSITION;
    float3 pos_w : POSITION;
    float3 nor_w : NORMAL;
    float2 uv_w : UV;
};

struct Material
{
    float4 diffuseAlbedo;
    float3 fresenlR0;
    float shininess;
};

struct Light
{
    float3 lightColor;
    float fallOffStart;
    float3 direction;
    float fallOffEnd;
    float3 position;
    float spotAngle;
};