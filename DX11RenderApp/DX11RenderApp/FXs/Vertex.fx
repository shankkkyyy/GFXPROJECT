

/*Defination of Vertex*/

struct VertexIn
{
	float3 pos_l   :   POSITION;
	float3 nor_l   :   NORMAL;
	float2 uv_l    :   TEXCOOD;
};				   	   
				   	   
struct VertexOut   	   
{				   	   
	float4 pos_h     :   SV_POSITION;
	float3 pos_w     :   POSITION;
	float3 nor_w     :   NORMAL;
	float2 uv_w      :   TEXCOOD;

};