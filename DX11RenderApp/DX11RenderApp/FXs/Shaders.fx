#include "Vertex.fx"
#include "Lighting.fx"

cbuffer perFrame
{
	DirectionalLight gDirectLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
	float3 gEyePos_w;
};

cbuffer perObject
{
	float4x4 gWorldViewProj_w; // for vs
	float4x4 gWorld_w;
	float4x4 gWorldInvTranspose_w;
	float4x4 gTexTransform_w;
	Material gMaterial;
};

Texture2D gTexMap;
 
SamplerState gAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;

};


VertexOut VS(VertexIn vin) 
{
	VertexOut vout;
	
	vout.pos_h = mul(float4(vin.pos_l, 1.0f), gWorldViewProj_w);
	vout.pos_w = mul(float4(vin.pos_l, 1.0f), gWorld_w).xyz;
	vout.nor_w = mul(vin.nor_l, (float3x3)gWorldInvTranspose_w);
	vout.nor_w = normalize(vout.nor_w);
	vout.uv_w = vin.uv_l;
		//mul(float4(vin.uv_l, 0.0f, 1.0f), gTexTransform_w).xy;


	return vout;
}

float4 PS(VertexOut pin, uniform bool bUseTex) : SV_TARGET
{

	float3 toEye = gEyePos_w - pin.pos_w;
	toEye = normalize(toEye);

	float4
		ambient = { 0,0,0,0 },
		diffuse = { 0,0,0,0 },
		specular = { 0,0,0,0 };

	float4 A, D, S; // contribution for each light source
	ComputeDirectionalLight(gDirectLight, gMaterial, pin.nor_w, toEye, A, D, S);

	ambient += A;
	diffuse += D;
	specular += S;

	ComputePointLight(gPointLight, gMaterial, pin.nor_w, toEye, pin.pos_w, A, D, S);

	ambient += A;
	diffuse += D;
	specular += S;

	ComputeSpotLight(gSpotLight, gMaterial, pin.nor_w, toEye, pin.pos_w, A, D, S);

	ambient += A;
	diffuse += D;
	specular += S;

	float4 texColor = { 1,1,1,1 };
	[flatten]
	if (bUseTex)
	{
		texColor = gTexMap.Sample(gAnisotropic, pin.uv_w);
	}

	float4 litColor = { 0,0,0,0 };
	litColor = texColor * (ambient + diffuse) + specular;
	litColor.a = gMaterial.diffuse.a * texColor.a;
	//litColor = ambient + diffuse + specular;
	//litColor.a = gMaterial.diffuse.a;

	return litColor;

}




technique11 lightTech
{
	pass p0 
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(true)));
	}

}

technique11 lightTechNoTex
{
	pass p0 
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(false)));

	}
}