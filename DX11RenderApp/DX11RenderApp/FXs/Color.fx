

cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
};

struct VertexIn
{
	float3 pos  :POSITION;
	float4 color: COLOR;
};

struct VertexOut
{
	float4 posH :SV_POSITION;
	float4 color:COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.posH = mul(float4(vin.pos, 1.0f), gWorldViewProj);
	vout.color = vin.color;
	return vout;
}

float4 PS(VertexOut pin) :SV_TARGET
{
	return pin.color;
}

technique11 colorTech
{
	pass p0 
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
};

