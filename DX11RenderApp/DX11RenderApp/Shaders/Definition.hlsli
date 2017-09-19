
struct VertexOutRTT
{
    float4 pos_h : SV_Position;
    float2 uv_l : UV;
};

struct VertexInInst
{
    float3 pos_l    : POSITION;
    float3 nor_l    : NORMAL;
    float2 uv_l     : UV;
    float4x4 world  : WORLD;
    uint instanceID : SV_InstanceID;
};

struct VertexInPos
{
    float3 pos_l : POSITION;
};

struct VertexTerrian
{
    float3 pos_l : POSITION;
    float2 uv    : UV;
};

struct DomainOut
{
    float4 pos_h : SV_POSITION;
};

struct VertexOutPos
{
    float4 pos_h : SV_Position;
    float3 pos_l : POSITIONL;
    
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

struct VertexIOBB
{
    float3 root_w : POSITION;
    float2 size   : SIZE;
};

struct GeoOutBB
{
    float4 pos_h : SV_POSITION;
    float3 pos_w : POSITION;
    float3 nor_w : NORMAL;
    float2 uv    : UV;
    uint primID  : SV_PrimitiveID;
};


struct Material
{
    float4 diffuseAlbedo;
    float3 fresenlR0;
    float  shininess;
    float4 reflection;
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

#define RED 0x52
#define GREEN 0x47
#define BLUE 0x42
